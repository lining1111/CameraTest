//
// Created by lining on 12/22/21.
//

#ifndef _MYVIDEOCAPTUREUSB_H
#define _MYVIDEOCAPTUREUSB_H

#include "MyVideoCapture.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
class MyVideoCaptureUSB : public MyVideoCapture {
public:
    string path = "/dev/video0";//默认使用video0
#define FrameBufSize 4//映射后，用户空间首地址个数
    typedef struct {
        unsigned char *ptr;//映射后，用户空间首地址
        unsigned int len;//长度
    } UserFrame;
    UserFrame userFrame[FrameBufSize];//映射后，用户空间首地址

public:
    MyVideoCaptureUSB(int index);

    ~MyVideoCaptureUSB();

public:
    int Open(void) final;

    int Close(void) final;

    int GetPixelWidthHeight(uint32_t &width, uint32_t &height) final;

    int SetPixelWidthHeight(uint32_t width, uint32_t height) final;

    int GetSupportFormat(vector<uint32_t> &supportFormat) final;

    int GetCurFormat(uint32_t &pixelFormat) final;

    int SetCurFormat(uint32_t pixelFormat) final;

    int GetFps(uint32_t &fps) final;

    int SetFps(uint32_t fps) final;

    int StartCapture(void) final;

    int GetImg(vector<unsigned char> &vector_data, timeval &tv) final;

    int StopCapture(void) final;


};

#ifdef __cplusplus
}
#endif

#endif //_MYVIDEOCAPTUREUSB_H
