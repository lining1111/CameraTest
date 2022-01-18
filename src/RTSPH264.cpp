//
// Created by lining on 1/13/22.
//

#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include "RTSPH264.h"

RTSPH264::RTSPH264() {
    camera = new Camera();
}

RTSPH264::~RTSPH264() {
    if (isOpen) {
        Close();
    }
    delete camera;
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
    //TODO 可以做一些环境检测功能

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
    //关闭camera
    camera->Destroy();
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

    //1.打开camera
    rtsph264->camera->Init();

    //将摄像头的帧数据写入到fifo
    while (rtsph264->isOpen) {
//        usleep(20 * 1000);
        rtsph264->camera->GetNextFrame();
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
    stream = rtsph264->camera->fifoName;
#endif

    //TODO 多播情况下，windows接收不到数据
#if 1
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
                                                            rtsph264->streamName.c_str(),
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
#else
    // Begin by setting up our usage environment:
    rtsph264->scheduler = BasicTaskScheduler::createNew();
    rtsph264->env = BasicUsageEnvironment::createNew(*rtsph264->scheduler);

    UserAuthenticationDatabase *authDB = NULL;
#ifdef ACCESS_CONTROL
    // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

    // Create the RTSP server:
    RTSPServer *rtspServer = RTSPServer::createNew(*rtsph264->env, 8554, authDB);
    if (rtspServer == NULL) {
        *rtsph264->env << "Failed to create RTSP server: " << rtsph264->env->getResultMsg() << "\n";
        exit(1);
    }

    char const *descriptionString
            = "Session streamed by \"testOnDemandRTSPServer\"";


    {
        ServerMediaSession *sms
                = ServerMediaSession::createNew(*rtsph264->env, rtsph264->streamName.c_str(),
                                                rtsph264->streamName.c_str(),
                                                descriptionString);
        Boolean reuseFirstSource = False;
        sms->addSubsession(H264VideoFileServerMediaSubsession
                           ::createNew(*rtsph264->env, stream.c_str(), reuseFirstSource));
        rtspServer->addServerMediaSession(sms);

        announceURL(rtspServer, sms);
    }
    if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) ||
        rtspServer->setUpTunnelingOverHTTP(8080)) {
        *rtsph264->env << "\n(We use port " << rtspServer->httpServerPortNum()
                       << " for optional RTSP-over-HTTP tunneling.)\n";
    } else {
        *rtsph264->env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
    }

    rtsph264->env->taskScheduler().doEventLoop(); // does not return
#endif

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
    stream = rtsph264->camera->fifoName;
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
