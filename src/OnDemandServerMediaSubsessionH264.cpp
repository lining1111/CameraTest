//
// Created by lining on 1/19/22.
//

#include <H264VideoStreamFramer.hh>
#include <FramedSourceH264.h>
#include <H264VideoRTPSink.hh>
#include <string>
#include "OnDemandServerMediaSubsessionH264.h"

OnDemandServerMediaSubsessionH264::OnDemandServerMediaSubsessionH264(UsageEnvironment &env, FramedSource *source,
                                                                     string fifoName, int fps)
        : OnDemandServerMediaSubsession(env, True) {
    if (!fifoName.empty()) {
        this->fifoName = fifoName;
    }
    if (fps > 0) {
        this->fps = fps;
    }

    m_pSource = source;
    m_pSDPLine = nullptr;
}

OnDemandServerMediaSubsessionH264::~OnDemandServerMediaSubsessionH264() {
    if (m_pSDPLine) {
        free(m_pSDPLine);
    }
}

char const *OnDemandServerMediaSubsessionH264::getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource) {
    if (m_pSDPLine) {
        return m_pSDPLine;
    }

    m_pDummpRTPSink = rtpSink;
    m_pDummpRTPSink->startPlaying(*inputSource, nullptr, nullptr);

    chkForAuxSDPLine(this);

    m_done = 0;
    envir().taskScheduler().doEventLoop(&m_done);
    m_pSDPLine = strdup(m_pDummpRTPSink->auxSDPLine());
    m_pDummpRTPSink->stopPlaying();
    return m_pSDPLine;

}

FramedSource *
OnDemandServerMediaSubsessionH264::createNewStreamSource(unsigned int clientSessionId, unsigned int &estBitrate) {
    string fifo = this->fifoName;
    int fps = this->fps;
    //确定流是h264
    return H264VideoStreamFramer::createNew(envir(), new FramedSourceH264(envir(), fifo, fps));
}

RTPSink *
OnDemandServerMediaSubsessionH264::createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic,
                                                    FramedSource *inputSource) {
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

OnDemandServerMediaSubsessionH264 *
OnDemandServerMediaSubsessionH264::createNew(UsageEnvironment &env, FramedSource *source, string fifoName, int fps) {
    return new OnDemandServerMediaSubsessionH264(env, source, fifoName, fps);
}

void OnDemandServerMediaSubsessionH264::afterPlayingDummy(void *p) {
    auto subSession = (OnDemandServerMediaSubsessionH264 *) p;
    subSession->m_done = 0xff;
}

void OnDemandServerMediaSubsessionH264::chkForAuxSDPLine(void *p) {
    auto subSession = (OnDemandServerMediaSubsessionH264 *) p;
    subSession->chkForAuxSDPLine1();
}

void OnDemandServerMediaSubsessionH264::chkForAuxSDPLine1() {
    if (m_pDummpRTPSink->auxSDPLine()) {
        m_done = 0xff;
    } else {
        double delay = 1000.0 / (this->fps);//ms
        int to_delay = delay * 1000;
        nextTask() = envir().taskScheduler().scheduleDelayedTask(to_delay, chkForAuxSDPLine, this);
    }
}
