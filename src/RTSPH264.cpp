//
// Created by lining on 1/13/22.
//

#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include "RTSPH264.h"
#include "FramedSourceH264.h"
#include "OnDemandServerMediaSubsessionH264.h"

RTSPH264::RTSPH264() {
    camera = new Camera();
}

RTSPH264::RTSPH264(Camera::Config cameraConf) {
    camera = new Camera(cameraConf);
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
    rtspServer->removeServerMediaSession(sms);
    Medium::close(rtspServer);
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

    // if 1下是自己实现的subssession和framedSource，在这两个基础上可以有改进的东西，两个是live555上的子类
#if 1
    //1 env
    rtsph264->scheduler = BasicTaskScheduler::createNew();
    rtsph264->env = BasicUsageEnvironment::createNew(*rtsph264->scheduler);

    auto env = rtsph264->env;

    // Note: This starts RTCP running automatically
    UserAuthenticationDatabase *authDB = nullptr;
    if (rtsph264->isNeedAuth) {
        authDB = new UserAuthenticationDatabase;
        authDB->addUserRecord(rtsph264->usr.c_str(), rtsph264->passwd.c_str());
    }
    //Create the RTSP server:
    rtsph264->rtspServer = RTSPServer::createNew(*env, 8554, authDB);
    if (rtsph264->rtspServer == nullptr) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

    //Add live stream
    FramedSourceH264 *framedSource = nullptr;

    rtsph264->sms = ServerMediaSession::createNew(*env, rtsph264->streamName.c_str(),
                                                  rtsph264->streamName.c_str(),
                                                  "Session streamed");
    rtsph264->sms->addSubsession(OnDemandServerMediaSubsessionH264::createNew(*rtsph264->env, framedSource,
                                                                              rtsph264->camera->fifoName,
                                                                              rtsph264->camera->fps));

    rtsph264->rtspServer->addServerMediaSession(rtsph264->sms);
    announceURL(rtsph264->rtspServer, rtsph264->sms);

    // Start the streaming:
    *env << "Beginning streaming...\n";

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
    rtsph264->rtspServer = RTSPServer::createNew(*rtsph264->env, 8554, authDB);
    if (rtsph264->rtspServer == nullptr) {
        *rtsph264->env << "Failed to create RTSP server: " << rtsph264->env->getResultMsg() << "\n";
        exit(1);
    }

    char const *descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";


    {
        rtsph264->sms = ServerMediaSession::createNew(*rtsph264->env, rtsph264->streamName.c_str(),
                                                      rtsph264->streamName.c_str(),
                                                      descriptionString);
        Boolean reuseFirstSource = False;
        rtsph264->sms->addSubsession(H264VideoFileServerMediaSubsession
                                     ::createNew(*rtsph264->env, stream.c_str(), reuseFirstSource));
        rtsph264->rtspServer->addServerMediaSession(rtsph264->sms);

        announceURL(rtsph264->rtspServer, rtsph264->sms);
    }
    if (rtsph264->rtspServer->setUpTunnelingOverHTTP(80) ||
        rtsph264->rtspServer->setUpTunnelingOverHTTP(8000) ||
        rtsph264->rtspServer->setUpTunnelingOverHTTP(8080)) {
        *rtsph264->env << "\n(We use port " << rtsph264->rtspServer->httpServerPortNum()
                       << " for optional RTSP-over-HTTP tunneling.)\n";
    } else {
        *rtsph264->env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
    }

    rtsph264->env->taskScheduler().doEventLoop(); // does not return
#endif

}
