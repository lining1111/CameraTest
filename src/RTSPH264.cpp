//
// Created by lining on 1/13/22.
//

#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include "RTSPH264.h"

RTSPH264::RTSPH264() {

}

RTSPH264::~RTSPH264() {
    Close();
}

void RTSPH264::announceURL(RTSPServer *rtspServer, ServerMediaSession *sms) {
    if (rtspServer == NULL || sms == NULL) {
        return; // sanuty check
    }

    UsageEnvironment &env = rtspServer->envir();

    env << "Play this stream using the URL ";
    if (weHaveAnIPv4Address(env)) {
        char *url = rtspServer->ipv4rtspURL(sms);
        env << "\"" << url << "\"";
        delete[] url;
        if (weHaveAnIPv6Address(env)) env << " or ";
    }
    if (weHaveAnIPv6Address(env)) {
        char *url = rtspServer->ipv6rtspURL(sms);
        env << "\"" << url << "\"";
        delete[] url;
    }
    env << "\n";
}

int RTSPH264::Open() {
    //2. 创建fifo
    if (access(fifioPath.c_str(), F_OK) < 0) {
        if (mkfifo(fifioPath.c_str(), 0777) < 0) {
            *env << "mkfifo failed:" << strerror(errno);
            return -1;
        }
    }

    isOpen = true;

    return 0;
}

int RTSPH264::Run() {
    //创建2个线程，分别是写fifo线程，和读fifo线程(rtspServer)
    //1. fifo write
    thread_dumpFifo = thread(ThreadDumpFifo, this);
    thread_dumpFifo.detach();
    //2. rtsp server
    thread_rtspServer = thread(ThreadRtspServer, this);
    thread_rtspServer.detach();

//    ThreadRtspServer(this);
    return 0;
}

int RTSPH264::Close() {
    isOpen = false;
    //关闭fifo write
    close(fifoFdWr);
    //关闭 rtsp server
    env->reclaim();
    delete scheduler;

    return 0;
}

void RTSPH264::ThreadDumpFifo(void *p) {
    if (p == nullptr) {
        return;
    }
    auto rtsph264 = (RTSPH264 *) p;
    if (!rtsph264->isOpen) {
        cout << "rtsph264 is not open" << endl;
        return;
    }
    cout << "fifo write start" << endl;

    auto env = rtsph264->env;

    rtsph264->fifoFdWr = open(rtsph264->fifioPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (rtsph264->fifoFdWr < 0) {
        *env << "fail open fifo\n";
        exit(1);
    }

    //测试将test.264写入到fifo
    FILE *fileH264;
    fileH264 = fopen("./video/test.264", "r");
    if (fileH264 == nullptr) {
        *env << "test.264 not open\n";
        exit(1);
    }
    //写fifo容量有限，达到上限后，就不能再写了
    int i = 0;
    while (!feof(fileH264)) {
        unsigned char buffer[1024];
        int get;
        get = fread(buffer, 1, sizeof(buffer) / sizeof(buffer[0]), fileH264);
        int ret = write(rtsph264->fifoFdWr, buffer, get);
        i++;
        cout << "copy " << i << endl;
        if (ret != get) {
            cout << "write fifo fail" << endl;
        }
    }
    fseek(fileH264, 0, SEEK_SET);
    fclose(fileH264);


    //将摄像头的帧数据写入到fifo



    while (rtsph264->isOpen) {
        sleep(10);
    }

}

void RTSPH264::ThreadRtspServer(void *p) {
    if (p == nullptr) {
        return;
    }
    auto rtsph264 = (RTSPH264 *) p;
    if (!rtsph264->isOpen) {
        cout << "rtsph264 is not open" << endl;
        return;
    }
    cout << "rtsp server start" << endl;

    string stream;
#ifdef STREAM_DEBUG
    stream = "./video/test.264";
#else
    stream = rtsph264->fifioPath;
#endif


    //1 env
    rtsph264->scheduler = BasicTaskScheduler::createNew();
    rtsph264->env = BasicUsageEnvironment::createNew(*rtsph264->scheduler);

    // Create 'groupsocks' for RTP and RTCP:
    struct sockaddr_storage destinationAddress;
    destinationAddress.ss_family = AF_INET;
    ((struct sockaddr_in &) destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*rtsph264->env);
    // Note: This is a multicast address.  If you wish instead to stream
    // using unicast, then you should use the "testOnDemandRTSPServer"
    // test program - not this test program - as a model.
    const unsigned short rtpPortNum = 18888;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 255;

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    Groupsock rtpGroupsock(*rtsph264->env, destinationAddress, rtpPort, ttl);
    rtpGroupsock.multicastSendOnly(); // we're a SSM source
    Groupsock rtcpGroupsock(*rtsph264->env, destinationAddress, rtcpPort, ttl);
    rtcpGroupsock.multicastSendOnly(); // we're a SSM source

    // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
    OutPacketBuffer::maxSize = 100000;
    rtsph264->videoSink = H264VideoRTPSink::createNew(*rtsph264->env, &rtpGroupsock, 96);
    // Create (and start) a 'RTCP instance' for this RTP sink:
    const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen + 1];
    gethostname((char *) CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    rtsph264->rtcp = RTCPInstance::createNew(*rtsph264->env, &rtcpGroupsock,
                                             estimatedSessionBandwidth, CNAME,
                                             rtsph264->videoSink, NULL /* we're a server */,
                                             True /* we're a SSM source */);

    auto env = rtsph264->env;


    // Note: This starts RTCP running automatically
    UserAuthenticationDatabase *authDB = nullptr;
    if (rtsph264->isNeedAuth) {
        authDB = new UserAuthenticationDatabase;
        authDB->addUserRecord(rtsph264->usr.c_str(), rtsph264->passwd.c_str());
    }
    RTSPServer *rtspServer = RTSPServer::createNew(*env, 8554, authDB);
    if (rtspServer == nullptr) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

    ServerMediaSession *sms = ServerMediaSession::createNew(*env, rtsph264->streamName.c_str(),
                                                            stream.c_str(),
                                                            "Session streamed by \"testH264VideoStreamer\"",
                                                            True /*SSM*/);
    sms->addSubsession(PassiveServerMediaSubsession::createNew(*rtsph264->videoSink,
                                                               rtsph264->rtcp));
    rtspServer->addServerMediaSession(sms);
    announceURL(rtspServer, sms);

    // Start the streaming:
    *env << "Beginning streaming...\n";
    play(rtsph264);

    env->taskScheduler().doEventLoop(); // does not return

}

void RTSPH264::afterPlaying(void *clientData) {
    if (clientData == nullptr) {
        return;
    }
    auto rtsph264 = (RTSPH264 *) clientData;
    auto env = rtsph264->env;
    *env << "...done reading from file\n";
    rtsph264->videoSink->stopPlaying();
    Medium::close(rtsph264->videoSource);
    // Note that this also closes the input file that this source read from.

    // Start playing once again:
    play(clientData);
}

void RTSPH264::play(void *p) {
    if (p == nullptr) {
        return;
    }
    auto rtsph264 = (RTSPH264 *) p;
    auto env = rtsph264->env;
// Open the input file as a 'byte-stream file source':
    string stream;
#ifdef STREAM_DEBUG
    stream = "./video/test.264";
#else
    stream = rtsph264->fifioPath;
#endif
    ByteStreamFileSource *fileSource
            = ByteStreamFileSource::createNew(*env, stream.c_str());
    if (fileSource == NULL) {
        *env << "Unable to open file \"" << stream.c_str()
             << "\" as a byte-stream file source\n";
        exit(1);
    }

    FramedSource *videoES = fileSource;

    // Create a framer for the Video Elementary Stream:
    rtsph264->videoSource = H264VideoStreamFramer::createNew(*env, videoES);

    // Finally, start playing:
    *env << "Beginning to read from file...\n";
    rtsph264->videoSink->startPlaying(*rtsph264->videoSource, afterPlaying, rtsph264);
}
