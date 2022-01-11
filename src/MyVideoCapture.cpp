//
// Created by lining on 12/21/21.
//

#include <iostream>
#include <cstring>
#include "MyVideoCapture.h"
#include <opencv2/opencv.hpp>

using namespace cv;

#define IMGSHOW 0

MyVideoCapture::MyVideoCapture() {
    //设置相机能力全部为0
    name.clear();
    videoConMode = VCM_Unknown;
    capability.width = 0;
    capability.height = 0;
    capability.pixelFormat = 0;
    capability.supportFormat.clear();
    capability.supportFormat.reserve(0);
    capability.fps = 0;
}

MyVideoCapture::~MyVideoCapture() {

}

int
MyVideoCapture::GetOutInfo(MyVideoCapture::OutInfo &outInfo, struct timeval tv, uint32_t pixelFormat, uint32_t cols,
                           int rows, char *buf, int bufSize) {
//    vector<uint32_t> support{
//            V4L2_PIX_FMT_YUYV,
//            V4L2_PIX_FMT_RGB24,
//            V4L2_PIX_FMT_JPEG,
//    };
//
//    bool isSupport = false;
//    for (auto s: support) {
//        if (pixelFormat == s) {
//            isSupport = true;
//            break;
//        }
//    }
//    if (!isSupport) {
//        cout << "pixelformat not support:" << to_string(pixelFormat) << endl;
//        return -1;
//    }

    outInfo.status = 0;
    outInfo.width = cols;
    outInfo.height = rows;
    outInfo.recordTime.tv_sec = tv.tv_sec;
    outInfo.recordTime.tv_usec = tv.tv_usec;
    outInfo.jpgSize = bufSize;
    outInfo.jpgData = (char *) malloc(bufSize);
    memcpy(outInfo.jpgData, buf, bufSize);

//    //转换为指定格式的数据
//    //JPEG
//    switch (pixelFormat) {
//        case V4L2_PIX_FMT_RGB24: {
//            Mat src;//原始数据
//            Mat dst;//目的数据
//            src = Mat(img);
//            cvtColor(src, dst, COLOR_RGB2YUV);
//            //展示
//            if (IMGSHOW) {
//                imshow("img", dst);
//                waitKey(0);
//            }
//            //
//            outInfo.jpgSize = dst.cols * dst.rows * dst.elemSize();
//            outInfo.jpgData = (char *) malloc(outInfo.jpgSize);
//            memcpy(outInfo.jpgData, dst.data, outInfo.jpgSize);
//        }
//            break;
//        case V4L2_PIX_FMT_YUYV: {
//            outInfo.jpgSize = img.cols * img.rows * img.elemSize();
//            outInfo.jpgData = (char *) malloc(outInfo.jpgSize);
//            memcpy(outInfo.jpgData, img.data, outInfo.jpgSize);
//        }
//    }


    return 0;
}
