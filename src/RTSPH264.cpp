//
// Created by lining on 1/13/22.
//

#include <sys/stat.h>
#include <iostream>
#include <cassert>
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
    //1 env
    scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    //2. 创建fifo
    if (access(fifioPath.c_str(), F_OK) < 0) {
        if (mkfifo(fifioPath.c_str(), 0777) < 0) {
            *env << "mkfifo failed:" << strerror(errno);
            return -1;
        }
    }
    //3.创建 rtsp
    UserAuthenticationDatabase *authDB = nullptr;
    if (isNeedAuth) {
        authDB = new UserAuthenticationDatabase;
        authDB->addUserRecord(usr.c_str(), passwd.c_str());
    }
    // Create 'groupsocks' for RTP and RTCP:
    struct sockaddr_storage destinationAddress;
    destinationAddress.ss_family = AF_INET;
    ((struct sockaddr_in &) destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*env);
    // Note: This is a multicast address.  If you wish instead to stream
    // using unicast, then you should use the "testOnDemandRTSPServer"
    // test program - not this test program - as a model.
    const unsigned short rtpPortNum = 18888;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 255;

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
    rtpGroupsock.multicastSendOnly(); // we're a SSM source
    Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
    rtcpGroupsock.multicastSendOnly(); // we're a SSM source

    // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
    OutPacketBuffer::maxSize = 100000;
    videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
    // Create (and start) a 'RTCP instance' for this RTP sink:
    const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen + 1];
    gethostname((char *) CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    rtcp = RTCPInstance::createNew(*env, &rtcpGroupsock,
                                   estimatedSessionBandwidth, CNAME,
                                   videoSink, NULL /* we're a server */,
                                   True /* we're a SSM source */);
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
    while (!feof(fileH264)) {
        unsigned char buffer[512];
        int get;
        get = fread(buffer, sizeof(buffer), 1, fileH264);
        write(rtsph264->fifoFdWr, buffer, get);
    }
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
    auto env = rtsph264->env;


    // Note: This starts RTCP running automatically
    RTSPServer *rtspServer = RTSPServer::createNew(*env, 8554);
    if (rtspServer == nullptr) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

    ServerMediaSession *sms = ServerMediaSession::createNew(*env, rtsph264->streamName.c_str(),
                                                            rtsph264->fifioPath.c_str(),
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
    ByteStreamFileSource *fileSource
            = ByteStreamFileSource::createNew(*env, rtsph264->fifioPath.c_str());
    if (fileSource == NULL) {
        *env << "Unable to open file \"" << rtsph264->fifioPath.c_str()
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
