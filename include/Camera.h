//
// Created by lining on 1/17/22.
//

#ifndef _CAMERA_H
#define _CAMERA_H

#include <cstdio>
#include <x264.h>
#include <string>

using namespace std;

class Camera {
#define IMGSHOW 0
    typedef struct {
        x264_param_t *param;
        x264_t *handle;
        x264_picture_t *picture;
        x264_nal_t *nal;
    } Encoder;

    typedef struct {
        char *start;
        int length;
    } BUFTYPE;

public:
    int width = 640;
    int height = 480;
    string file_video = "/dev/video0";

    //camera fd
    int fd;
    //h246 buf
    char *h264Buf;
    int h264BufLen;
    unsigned int n_buffer;
    BUFTYPE *usr_buf;
    Encoder en;
    //fifo
    int pipe_fd;
    string fifoName = "./video_fifo";

public:
    Camera();

    Camera(int width, int height, string video, string fifoName);

    ~Camera();

public:

    //fifo相关
    int openPipe();

    int writePipe();

    int closePipe();

    //v4l2 相关操作

    void openCamera(void);

    void initCamera(void);

    void initMmap(void);

    void startCapture(void);

    int isCanRead(void);

    void readOneFrame(void);

    void stopCapture(void);

    void closeCamera(void);

    //x264 encoder
    void initEncoder(void);

    void compressBegin(Encoder *en, int width, int height);

    int compressFrame(Encoder *en, int type, char *in, int len, char *out);

    void compressEnd(Encoder *en);

    void closeEncoder(void);

    //程序流程
    // Init(openCamera---initCamera---initMmap---openPipe---initEncoder---startCapture)
    // ---GetNextFrame(cameraAbleRead---readOneFrame---compressFrame---写入pipe_fd)
    // ---Destroy(stopCapture---closeEncoder---closePipe---closeCamera)
    void Init();

    void GetNextFrame();

    void Destroy();
};

#endif //_CAMERA_H
