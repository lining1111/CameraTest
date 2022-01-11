//
// Created by lining on 12/14/21.
//

#include <fcntl.h>
#include <string>
#include "UvcTest.h"
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>

using namespace std;

static string cameraName = "/dev/video0";

int UvcTest(void) {
    int fdUsbCam = open(cameraName.c_str(), O_RDWR, 0);
    if (fdUsbCam == -1) {
        return -1;
    }
    struct v4l2_capability cap;
    int ret = ioctl(fdUsbCam, VIDIOC_QUERYCAP, &cap);
    if (ret != 0) {
        perror("ioctl VIDIOC_QUERYCAP");
        return -1;
    }

    //显示所有支持的格式
    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cout << "支持的格式：" << endl;

    while (ioctl(fdUsbCam, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
        cout << to_string(fmtdesc.index) << ":" << fmtdesc.description << endl;
        fmtdesc.index++;
    }

    //查看当前格式
    struct v4l2_format format;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fdUsbCam, VIDIOC_G_FMT, &format);
    cout << "当前数据格式信息：宽度：" + to_string(format.fmt.pix.width) +
            "，高度：" + to_string(format.fmt.pix.height) << endl;

    //设置当前格式

//    //获取和设置摄像头参数
//    struct v4l2_control ctl;
//    cout << "白平衡:" << endl;
//    ctl.id = V4L2_CID_AUTO_WHITE_BALANCE;
//    ctl.value = V4L2_WHITE_BALANCE_MANUAL;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取白平衡色温
//    ctl.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "白平衡色温：" << to_string(ctl.value) << endl;
//
//    //设置白平衡色温
//    ctl.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
//    ctl.value = 5100;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取亮度
//    cout << "亮度:" << endl;
//    ctl.id = V4L2_CID_BRIGHTNESS;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "亮度：" << to_string(ctl.value) << endl;
//
//    //设置亮度
//    ctl.id = V4L2_CID_BRIGHTNESS;
//    ctl.value = 40;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取对比度
//    cout << "对比度:" << endl;
//    ctl.id = V4L2_CID_CONTRAST;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "对比度：" << to_string(ctl.value) << endl;
//
//    //设置对比度
//    ctl.id = V4L2_CID_CONTRAST;
//    ctl.value = 40;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取饱和度
//    cout << "饱和度:" << endl;
//    ctl.id = V4L2_CID_SATURATION;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "饱和度：" << to_string(ctl.value) << endl;
//
//    //设置饱和度
//    ctl.id = V4L2_CID_CONTRAST;
//    ctl.value = 40;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取色度
//    cout << "色度:" << endl;
//    ctl.id = V4L2_CID_HUE;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "色度：" << to_string(ctl.value) << endl;
//
//    //设置色度
//    ctl.id = V4L2_CID_HUE;
//    ctl.value = 40;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取锐度
//    cout << "锐度:" << endl;
//    ctl.id = V4L2_CID_SHARPNESS;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "锐度：" << to_string(ctl.value) << endl;
//
//    //设置锐度
//    ctl.id = V4L2_CID_SHARPNESS;
//    ctl.value = 40;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取背光补偿
//    cout << "背光补偿:" << endl;
//    ctl.id = V4L2_CID_BACKLIGHT_COMPENSATION;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "背光补偿：" << to_string(ctl.value) << endl;
//
//    //设置背光补偿
//    ctl.id = V4L2_CID_BACKLIGHT_COMPENSATION;
//    ctl.value = 40;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//
//    //获取伽玛值
//    cout << "伽玛值:" << endl;
//    ctl.id = V4L2_CID_GAMMA;
//
//    if (ioctl(fdUsbCam, VIDIOC_G_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }
//    cout << "伽玛值：" << to_string(ctl.value) << endl;
//
//    //设置伽玛值
//    ctl.id = V4L2_CID_GAMMA;
//    ctl.value = 40;
//
//    if (ioctl(fdUsbCam, VIDIOC_S_CTRL, &ctl) == -1) {
//        perror("ioctl");
//        return -1;
//    }

    close(fdUsbCam);
    return 0;
}
