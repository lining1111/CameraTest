//
// Created by lining on 1/17/22.
//

#include <linux/videodev2.h>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Camera.h"


Camera::Camera() {

}

Camera::Camera(int width, int height, string video, string fifoName) {
    this->width = width;
    this->height = height;
    this->file_video = video;
    this->fifoName = fifoName;
}


Camera::~Camera() {

}

int Camera::openPipe() {
    //1. 创建fifo
    if (access(fifoName.c_str(), F_OK) < 0) {
        if (mkfifo(fifoName.c_str(), 0777) < 0) {
            cout << "mkfifo failed:" << strerror(errno) << endl;
            return -1;
        }
    }
    //2. 打开fifo
    pipe_fd = open(fifoName.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0777);
    if (pipe_fd == -1) {
        cout << "fail open fifo" << endl;
        return -1;
    }

    return 0;
}

int Camera::writePipe() {
    int ret = 0;
    if (h264BufLen > 0) {
        ret = write(pipe_fd, h264Buf, h264BufLen);
        if (ret = h264BufLen) {
            cout << "frame len write:" << h264BufLen << endl;
        }
    }
    return ret;
}

int Camera::closePipe() {
    close(pipe_fd);
    return 0;
}

void Camera::openCamera(void) {
    struct v4l2_input inp;

    fd = open(file_video.c_str(), O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) {
        fprintf(stderr, "%s open err \n", file_video.c_str());
        exit(EXIT_FAILURE);
    };

    inp.index = 0;
    if (-1 == ioctl(fd, VIDIOC_S_INPUT, &inp)) {
        fprintf(stderr, "VIDIOC_S_INPUT \n");
    }

}

void Camera::initCamera(void) {
    struct v4l2_capability cap;
    struct v4l2_format tv_fmt;
    struct v4l2_fmtdesc fmtdesc;
    int ret;

    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        fprintf(stderr, "fail to ioctl VIDEO_QUERYCAP \n");
        exit(EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
        fprintf(stderr, "The Current device is not a video capture device \n");
        exit(EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        printf("The Current device does not support streaming i/o\n");
        exit(EXIT_FAILURE);
    }

    printf("\ncamera driver name is : %s\n", cap.driver);
    printf("camera device name is : %s\n", cap.card);
    printf("camera bus information: %s\n", cap.bus_info);

    tv_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    tv_fmt.fmt.pix.width = width;
    tv_fmt.fmt.pix.height = height;
    tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    tv_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if (ioctl(fd, VIDIOC_S_FMT, &tv_fmt) < 0) {
        fprintf(stderr, "VIDIOC_S_FMT set err\n");
        close(fd);
        exit(-1);
    }
}

void Camera::initMmap(void) {
    struct v4l2_requestbuffers reqbufs;
    memset(&reqbufs, 0, sizeof(reqbufs));
    reqbufs.count = 4;
    reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbufs.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(fd, VIDIOC_REQBUFS, &reqbufs)) {
        perror("Fail to ioctl 'VIDIOC_REQBUFS'");
        exit(EXIT_FAILURE);
    }

    n_buffer = reqbufs.count;
    printf("n_buffer = %d\n", n_buffer);
    usr_buf = (BUFTYPE *) calloc(reqbufs.count, sizeof(BUFTYPE));
    if (usr_buf == NULL) {
        printf("Out of memory\n");
        exit(-1);
    }

    for (n_buffer = 0; n_buffer < reqbufs.count; ++n_buffer) {

        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffer;

        if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf)) {
            perror("Fail to ioctl : VIDIOC_QUERYBUF");
            exit(EXIT_FAILURE);
        }

        usr_buf[n_buffer].length = buf.length;
        usr_buf[n_buffer].start = (char *) mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (MAP_FAILED == usr_buf[n_buffer].start) {
            perror("Fail to mmap");
            exit(EXIT_FAILURE);
        }

    }
}

void Camera::startCapture(void) {
    unsigned int i;
    enum v4l2_buf_type type;

    for (i = 0; i < n_buffer; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
            perror("Fail to ioctl 'VIDIOC_QBUF'");
            exit(EXIT_FAILURE);
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(fd, VIDIOC_STREAMON, &type)) {
        printf("i=%d.\n", i);
        perror("VIDIOC_STREAMON");
        close(fd);
        exit(EXIT_FAILURE);
    }

}

int Camera::isCanRead(void) {
    fd_set fds;
    struct timeval tv;
    int ret;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);            /*Timeout*/
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    ret = select(fd + 1, &fds, NULL, NULL, &tv);

    if (-1 == ret) {
        if (EINTR == errno) {
            return -1;
        }

        perror("Fail to select");
        exit(EXIT_FAILURE);
    }

    if (0 == ret) {
        fprintf(stderr, "select Timeout\n");
        //exit(-1);
    }

    return ret;
}

void Camera::readOneFrame(void) {
    struct v4l2_buffer buf;

    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
        perror("Fail to ioctl 'VIDIOC_DQBUF'");
        exit(EXIT_FAILURE);
    }
    assert(buf.index < n_buffer);

    //opencv show img
    if (IMGSHOW) {
        cv::Mat img;
        img.create(height, width, CV_8UC2);
        memcpy(img.data, usr_buf[buf.index].start, usr_buf[buf.index].length);
        cv::Mat imgShow;
        cvtColor(img, imgShow, cv::COLOR_YUV2BGR_YUYV);
        cv::imshow("imgShow", imgShow);
        cv::waitKey(0);
    }


    h264BufLen = compressFrame(&en, -1, usr_buf[buf.index].start, usr_buf[buf.index].length, h264Buf);

    if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
        perror("Fail to ioctl 'VIDIOC_QBUF'");
        exit(EXIT_FAILURE);
    }
}

void Camera::stopCapture(void) {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type)) {
        perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
        exit(EXIT_FAILURE);
    }
}

void Camera::closeCamera(void) {
    unsigned int i;
    for (i = 0; i < n_buffer; i++) {
        if (-1 == munmap(usr_buf[i].start, usr_buf[i].length)) {
            exit(-1);
        }
    }

    free(usr_buf);
    if (-1 == close(fd)) {
        perror("Fail to close fd");
        exit(EXIT_FAILURE);
    }
}

void Camera::initEncoder(void) {
    compressBegin(&en, width, height);
    h264Buf = (char *) malloc(width * height * 2);
}

void Camera::compressBegin(Camera::Encoder *en, int width, int height) {
    en->param = (x264_param_t *) malloc(sizeof(x264_param_t));
    en->picture = (x264_picture_t *) malloc(sizeof(x264_picture_t));
    x264_param_default(en->param); //set default param
    //en->param->rc.i_rc_method = X264_RC_CQP;
    en->param->i_log_level = X264_LOG_DEBUG;

    en->param->i_threads = X264_SYNC_LOOKAHEAD_AUTO;
    en->param->i_width = width; //set frame width
    en->param->i_height = height; //set frame height

    //en->param->i_frame_total = 0;

    //en->param->i_keyint_max = 10;
    en->param->rc.i_lookahead = 0;//表示I帧向前缓冲区
    //en->param->i_bframe = 5;

    //en->param->b_open_gop = 0;
    //en->param->i_bframe_pyramid = 0;
    //en->param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;

    //en->param->rc.i_bitrate = 1024 * 10;//rate 10 kbps
    en->param->i_fps_num = 30;//帧率分子
    en->param->i_fps_den = 1;//帧率分母
    en->param->i_csp = X264_CSP_I422;

    x264_param_apply_profile(en->param, x264_profile_names[4]);

    if ((en->handle = x264_encoder_open(en->param)) == 0) {
        return;
    }
    /* Create a new pic */
    x264_picture_alloc(en->picture, X264_CSP_I422, en->param->i_width,
                       en->param->i_height);
}

int Camera::compressFrame(Camera::Encoder *en, int type, char *in, int len, char *out) {
    x264_picture_t pic_out;
    int index_y, index_u, index_v;
    int num;
    int nNal = -1;
    int result = 0;
    int i = 0;
    static long int pts = 0;
    char *p_out = out;
    char *y = (char *) en->picture->img.plane[0];
    char *u = (char *) en->picture->img.plane[1];
    char *v = (char *) en->picture->img.plane[2];

    index_y = 0;
    index_u = 0;
    index_v = 0;

    num = width * height * 2;

    for (i = 0; i < num; i = i + 4) {
        *(y + (index_y++)) = *(in + i);
        *(u + (index_u++)) = *(in + i + 1);
        *(y + (index_y++)) = *(in + i + 2);
        *(v + (index_v++)) = *(in + i + 3);
    }

    switch (type) {
        case 0:
            en->picture->i_type = X264_TYPE_P;
            break;
        case 1:
            en->picture->i_type = X264_TYPE_IDR;
            break;
        case 2:
            en->picture->i_type = X264_TYPE_I;
            break;
        default:
            en->picture->i_type = X264_TYPE_AUTO;
            break;
    }

    en->picture->i_pts = pts++;

    if (x264_encoder_encode(en->handle, &(en->nal), &nNal, en->picture,
                            &pic_out) < 0) {
        return -1;
    }

    for (i = 0; i < nNal; i++) {
        memcpy(p_out, en->nal[i].p_payload, en->nal[i].i_payload);
        p_out += en->nal[i].i_payload;
        result += en->nal[i].i_payload;
    }

    return result;
}

void Camera::compressEnd(Camera::Encoder *en) {
    if (en->handle) {
        x264_encoder_close(en->handle);
    }
    if (en->picture) {
        x264_picture_clean(en->picture);
        free(en->picture);
        en->picture = 0;
    }
    if (en->param) {
        free(en->param);
        en->param = 0;
    }
}

void Camera::closeEncoder(void) {
    compressEnd(&en);
    free(h264Buf);
}

void Camera::Init() {
    openCamera();
    initCamera();
    initMmap();
    openPipe();
    initEncoder();
    startCapture();
}

void Camera::GetNextFrame(void) {
    int ret;
    ret = isCanRead();
    if (ret > 0) {

        readOneFrame();
//        fwrite(this->h264Buf, this->h264BufLen, 1, this->pipe_fd);
        writePipe();
    } else {

    }
}

void Camera::Destroy() {
    stopCapture();
    closeEncoder();
    closePipe();
    closeCamera();
}
