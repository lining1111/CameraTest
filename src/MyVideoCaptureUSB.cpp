//
// Created by lining on 12/22/21.
//

#include "MyVideoCaptureUSB.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

MyVideoCaptureUSB::MyVideoCaptureUSB(int index) {
    this->path = "/dev/video" + to_string(index);
    this->videoConMode = VCM_USB;
}

MyVideoCaptureUSB::~MyVideoCaptureUSB() {

}

int MyVideoCaptureUSB::Open(void) {
    this->fd = open(this->path.c_str(), O_RDWR);
    if (this->fd == -1) {
        cout << this->path << ":open fail,error:" << strerror(errno) << endl;
        isOpen = false;
        return -1;
    }
    //获取基本信息
    struct v4l2_capability capability;
    if (ioctl(this->fd, VIDIOC_QUERYCAP, &capability) < 0) {
        cout << "get video info fail" << endl;
        return -1;
    }
    cout << "Driver Name:" << capability.driver << "Card Name:" << capability.card << "Bus Info:" << capability.bus_info
         << endl;
    if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE != V4L2_CAP_VIDEO_CAPTURE) {
        cout << "device not support capture" << endl;
        return -1;
    }

    if (capability.capabilities & V4L2_CAP_STREAMING != V4L2_CAP_STREAMING) {
        cout << "device not support streaming" << endl;
        return -1;
    }

    isOpen = true;

    return 0;
}

int MyVideoCaptureUSB::Close(void) {
    //如果开启图像抓取，则关闭
    if (isCapture) {
        StopCapture();
    }
    close(this->fd);
    isOpen = false;
    return 0;
}

int MyVideoCaptureUSB::GetPixelWidthHeight(uint32_t &width, uint32_t &height) {
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(this->fd, VIDIOC_G_FMT, &format) == -1) {
        return -1;
    }
    width = format.fmt.pix.width;
    height = format.fmt.pix.height;

    return 0;
}

int MyVideoCaptureUSB::SetPixelWidthHeight(uint32_t width, uint32_t height) {
    struct v4l2_format format;
    //1.先获取原始数据
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(this->fd, VIDIOC_G_FMT, &format) == -1) {
        return -1;
    }
    //2.设置数据
    format.fmt.pix.width = width;
    format.fmt.pix.height = height;
    if (ioctl(this->fd, VIDIOC_S_FMT, &format) == -1) {
        return -1;
    }
    //3.获取设置后的数据
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(this->fd, VIDIOC_G_FMT, &format) == -1) {
        return -1;
    }
    if (format.fmt.pix.width != width || format.fmt.pix.height != height) {
        return -1;
    }

    return 0;
}

int MyVideoCaptureUSB::GetSupportFormat(vector<uint32_t> &supportFormat) {
    supportFormat.clear();
    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = 0;
    do {
        ret = ioctl(this->fd, VIDIOC_ENUM_FMT, &fmtdesc);
        if (ret != -1) {
            supportFormat.push_back(fmtdesc.pixelformat);
            fmtdesc.index++;
        }
    } while (ret != -1);

    return 0;
}

int MyVideoCaptureUSB::GetCurFormat(uint32_t &pixelFormat) {
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(this->fd, VIDIOC_G_FMT, &format) == -1) {
        return -1;
    }
    pixelFormat = format.fmt.pix.pixelformat;

    return 0;
}

int MyVideoCaptureUSB::SetCurFormat(uint32_t pixelFormat) {
    struct v4l2_format format;
    //1.先获取原始数据
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(this->fd, VIDIOC_G_FMT, &format) == -1) {
        return -1;
    }
    //2.设置数据
    format.fmt.pix.pixelformat = pixelFormat;
    if (ioctl(this->fd, VIDIOC_S_FMT, &format) == -1) {
        return -1;
    }
    //3.获取设置后的数据
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(this->fd, VIDIOC_G_FMT, &format) == -1) {
        return -1;
    }
    if (format.fmt.pix.pixelformat != pixelFormat) {
        return -1;
    }
    return 0;
}

int MyVideoCaptureUSB::GetFps(uint32_t &fps) {
    struct v4l2_streamparm streamparm;
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(this->fd, VIDIOC_G_PARM, &streamparm) == -1) {
        return -1;
    }
    fps = streamparm.parm.capture.timeperframe.denominator / streamparm.parm.capture.timeperframe.numerator;

    return 0;
}

int MyVideoCaptureUSB::SetFps(uint32_t fps) {
    struct v4l2_streamparm streamparm;
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //1.先获取原始数据
    if (ioctl(this->fd, VIDIOC_G_PARM, &streamparm) == -1) {
        return -1;
    }
    //2.设置数据
    streamparm.parm.capture.timeperframe.numerator = 1;
    streamparm.parm.capture.timeperframe.denominator = fps;
    if (ioctl(this->fd, VIDIOC_S_PARM, &streamparm) == -1) {
        return -1;
    }
    //3.获取数据对比
    if (ioctl(this->fd, VIDIOC_G_PARM, &streamparm) == -1) {
        return -1;
    }
    if (fps != (streamparm.parm.capture.timeperframe.denominator / streamparm.parm.capture.timeperframe.numerator)) {
        return -1;
    }

    return 0;
}

int MyVideoCaptureUSB::StartCapture(void) {
    int ret = 0;
    //1.申请内核空间
    struct v4l2_requestbuffers requestbuffers;
    requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestbuffers.count = FrameBufSize; //申请4个缓冲区
    requestbuffers.memory = V4L2_MEMORY_MMAP;//映射方式
    if (ioctl(fd, VIDIOC_REQBUFS, &requestbuffers) < 0) {
        perror("VIDIOC_REQBUFS is error\n");
        return -1;
    }
    //2.映射
    struct v4l2_buffer mapbuffer;
    //初始化type, index
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for (int i = 0; i < FrameBufSize; i++) {
        mapbuffer.index = i;
        ret = ioctl(fd, VIDIOC_QUERYBUF, &mapbuffer);//从内核空间中查询一个空间做映射
        if (ret < 0) {
            perror("VIDIOC_QUERYBUF is error\n");
            return -1;
        }
        userFrame[i].ptr = (unsigned char *) mmap(NULL, mapbuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                                  mapbuffer.m.offset);
        userFrame[i].len = mapbuffer.length;

        //通知使用完毕--‘放回去’
        ret = ioctl(fd, VIDIOC_QBUF, &mapbuffer);
        if (ret < 0) {
            perror("VIDIOC_QBUF is error\n");
            return -1;
        }
    }

    //6.start
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        perror("VIDIOC_STREAMON is error\n");
        return -1;
    }
    isCapture = true;

    return 0;
}

int MyVideoCaptureUSB::GetImg(vector<unsigned char> &vector_data, timeval &tv) {
    int ret = 0;
    //1.从队列中提取一帧数据
    struct v4l2_buffer buffer;
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_DQBUF, &buffer);
    if (ret < 0) {
        perror("VIDIOC_DQBUF is error\n");
        return -1;
    }
    //2.拷贝数据
    for (int i = 0; i < userFrame[buffer.index].len; i++) {
        vector_data.push_back(userFrame[buffer.index].ptr[i]);
    }
    ioctl(fd, VIDIOC_QBUF, buffer);
    gettimeofday(&tv, nullptr);

    return 0;
}

int MyVideoCaptureUSB::StopCapture(void) {
    if (!isCapture) {
        return 0;
    }

    int ret = 0;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);

    for (int i = 0; i < FrameBufSize; i++) {
        munmap(userFrame[i].ptr, userFrame[i].len);
    }
    isCapture = true;

    return 0;
}
