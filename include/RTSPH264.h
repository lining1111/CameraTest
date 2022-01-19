//
// Created by lining on 1/13/22.
//

#ifndef _RTSPH264_H
#define _RTSPH264_H

#include <string>
#include <UsageEnvironment/UsageEnvironment.hh>
#include <liveMedia/liveMedia.hh>

#include <BasicUsageEnvironment/BasicUsageEnvironment.hh>
#include <groupsock/GroupsockHelper.hh>
#include <thread>

#include "Camera.h"

using namespace std;

class RTSPH264 {
public:
    //rtsp server相关
    TaskScheduler *scheduler = nullptr;
    UsageEnvironment *env = nullptr;
    string streamName = "testStream";
    RTSPServer *rtspServer = nullptr;
    ServerMediaSession *sms = nullptr;
    // rtsp server验证信息
    bool isNeedAuth = false;
    string usr = "username1";
    string passwd = "password1";

    //camera相关
    Camera *camera = nullptr;
    string cameraDev = "/dev/video";
    int picWidth = 640;
    int picHeight = 480;
    int fps = 30;

    //2个线程
    bool isOpen = false;
    thread thread_dumpFifo;//向fifo内填充图像
    thread thread_rtspServer;//启动rtsp服务端

public:
    RTSPH264();
    RTSPH264(Camera::Config cameraConf);

    ~RTSPH264();

public:
    int Open();

    int Run();

    int Close();

private:
    static void announceURL(RTSPServer *rtspServer, ServerMediaSession *sms);

    static void ThreadDumpFifo(void *p);

    static void ThreadRtspServer(void *p);
};


#endif //_RTSPH264_H
