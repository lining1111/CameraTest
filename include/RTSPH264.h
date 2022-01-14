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

using namespace std;

class RTSPH264 {
public:
    string fifioPath = "./video_fifo";//fifo的位置
    int fifoFdWr = 0;
    TaskScheduler *scheduler = nullptr;
    UsageEnvironment *env = nullptr;
    string streamName = "testStream";
    H264VideoStreamFramer *videoSource = nullptr;
    RTPSink *videoSink = nullptr;
    RTCPInstance *rtcp = nullptr;

    //验证信息相关
    bool isNeedAuth = false;
    string usr = "username1";
    string passwd = "password1";

    //2个线程
    bool isOpen = false;
    thread thread_dumpFifo;//向fifo内填充图像
    thread thread_rtspServer;//启动rtsp服务端

//#define STREAM_DEBUG //是否开启rtsp server测试
public:
    RTSPH264();

    ~RTSPH264();

public:
    int Open();

    int Run();

    int Close();

private:
    static void announceURL(RTSPServer *rtspServer, ServerMediaSession *sms);

    static void ThreadDumpFifo(void *p);

    static void ThreadRtspServer(void *p);

    static void afterPlaying(void *clientData);

    static void play(void *p);

};


#endif //_RTSPH264_H
