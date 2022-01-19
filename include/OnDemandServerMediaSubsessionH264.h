//
// Created by lining on 1/19/22.
//

/**
 * OnDemandServerMediaSubsession的子类
 *
 */

#ifndef _ONDEMANDSERVERMEDIASUBSESSIONH264_H
#define _ONDEMANDSERVERMEDIASUBSESSIONH264_H


#include <OnDemandServerMediaSubsession.hh>
#include <cstring>

using namespace std;

class OnDemandServerMediaSubsessionH264 : public OnDemandServerMediaSubsession {
private:
    //FrameSource相关
    string fifoName = "./video_fifo";
    int fps = 30;

public:
    OnDemandServerMediaSubsessionH264(UsageEnvironment &env, FramedSource *source, string fifoName, int fps);

    ~OnDemandServerMediaSubsessionH264();

public:
    virtual char const *getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource);

    // new virtual functions, defined by all subclasses
    virtual FramedSource *createNewStreamSource(unsigned clientSessionId,
                                                unsigned &estBitrate);

    // "estBitrate" is the stream's estimated bitrate, in kbps
    virtual RTPSink *createNewRTPSink(Groupsock *rtpGroupsock,
                                      unsigned char rtpPayloadTypeIfDynamic,
                                      FramedSource *inputSource);

    static OnDemandServerMediaSubsessionH264 *
    createNew(UsageEnvironment &env, FramedSource *source, string fifoName, int fps);

    static void afterPlayingDummy(void *p);

    static void chkForAuxSDPLine(void *p);

    void chkForAuxSDPLine1();

private:
    FramedSource *m_pSource;
    char *m_pSDPLine;
    RTPSink *m_pDummpRTPSink;
    char m_done;

};


#endif //_ONDEMANDSERVERMEDIASUBSESSIONH264_H
