//
// Created by lining on 1/19/22.
//
/**
 * FramedSource的子类，自定义
 * 主要从固定的地方获取一帧数据
 *
 *
 */

#ifndef _FRAMEDSOURCEH264_H
#define _FRAMEDSOURCEH264_H


#include <FramedSource.hh>
#include <cstring>
#include <limits.h>
#include <string>

using namespace std;


class FramedSourceH264 : public FramedSource {

#define BUFFER_SIZE (1024*1024)
#define REV_BUF_SIZE (1024*1024)

private:
    string fifoName = "./video_fifo";
    int fps = 30;

public:
    FramedSourceH264(UsageEnvironment &env, string fifoName, int fps);

    ~FramedSourceH264();

public:
    // redefined virtual functions:
    virtual unsigned maxFrameSize() const;
    // size of the largest possible frame that we may serve, or 0
    // if no such maximum is known (default)

    virtual void doGetNextFrame();

    static void getNextFrame(void *p);

    void GetFrameData();

private:
    void *m_pToken = nullptr;
    char *m_pFrameBuffer = nullptr;
    int m_hFifo = 0;

};


#endif //_FRAMEDSOURCEH264_H
