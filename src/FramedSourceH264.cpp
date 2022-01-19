//
// Created by lining on 1/19/22.
//

#include <fcntl.h>
#include <time.h>
#include <iostream>
#include "FramedSourceH264.h"

FramedSourceH264::FramedSourceH264(UsageEnvironment &env, string fifoName, int fps) : FramedSource(env) {
    if (!fifoName.empty()) {
        this->fifoName = fifoName;
    }
    if (fps > 0) {
        this->fps = fps;
    }

    m_hFifo = open(fifoName.c_str(), O_RDONLY);
    cout << "open fifo result= " << m_hFifo << endl;
    if (m_hFifo == -1) {
        return;
    }

    m_pFrameBuffer = new char[REV_BUF_SIZE];
    if (m_pFrameBuffer == nullptr) {
        cout << "error malloc data buffer failed" << endl;
        return;
    }
    memset(m_pFrameBuffer, 0, REV_BUF_SIZE);

}

FramedSourceH264::~FramedSourceH264() {
    if (m_hFifo > 0) {
        ::close(m_hFifo);
    }
    envir().taskScheduler().unscheduleDelayedTask(m_pToken);
    if (m_pFrameBuffer != nullptr) {
        delete[]m_pFrameBuffer;
        m_pFrameBuffer = nullptr;
    }
}

unsigned FramedSourceH264::maxFrameSize() const {
    //最大150000    #define BANK_SIZE 150000  H264VideoStreamParser
    return 1024 * 140;
}

void FramedSourceH264::doGetNextFrame() {
    //根据 fps，计算等待时间
    double delay = 1000.0 / (fps * 2);//ms
    int to_delay = delay * 1000;//us
    //添加延迟任务
    m_pToken = envir().taskScheduler().scheduleDelayedTask(to_delay, getNextFrame, this);
}

void FramedSourceH264::getNextFrame(void *p) {
    auto framedSource = (FramedSourceH264 *) p;
    framedSource->GetFrameData();
}

void FramedSourceH264::GetFrameData() {
    //获取时间戳
    gettimeofday(&fPresentationTime, nullptr);
    fFrameSize = 0;
    int len = 0;
    unsigned char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    (len = read(m_hFifo, buffer, sizeof(buffer) / sizeof(buffer[0])));
    memcpy(m_pFrameBuffer + fFrameSize, buffer, len);
    fFrameSize += len;
    cout << "GetFrameData len = " << len << "fMaxSize = " << fMaxSize << endl;

    //填充帧数据
    memcpy(fTo, m_pFrameBuffer, fFrameSize);

    if (fFrameSize > fMaxSize) {
        fNumTruncatedBytes = fFrameSize - fMaxSize;
        fFrameSize = fMaxSize;
    } else {
        fNumTruncatedBytes = 0;
    }
    afterGetting(this);
}
