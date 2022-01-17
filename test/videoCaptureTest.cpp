//
// Created by lining on 12/21/21.
//

#include "MyVideoCapture.h"
#include "MyVideoCaptureUSB.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs/legacy/constants_c.h"

using namespace cv;

using namespace std;

#define IMGSHOW 1


int main() {
    MyVideoCaptureUSB *videoCapture = new MyVideoCaptureUSB(0);
    videoCapture->Open();

    //要设置的分辨率 宽和高
    MyVideoCapture::Resolution resolution4k = {
            .width = 3840,
            .height = 2160,
    };
    uint32_t width = resolution4k.width;
    uint32_t height = resolution4k.height;
    //要设置的图片格式
    uint32_t pixelFormat = V4L2_PIX_FMT_MJPEG;

    vector<unsigned char> buffer;
    timeval tv;
    videoCapture->GetSupportFormat(videoCapture->capability.supportFormat);
    videoCapture->SetPixelWidthHeight(width, height);
    uint32_t curFps = 0;
    videoCapture->GetFps(curFps);
    cout << "fps:" << to_string(curFps) << endl;
    videoCapture->SetCurFormat(pixelFormat);
    videoCapture->GetCurFormat(pixelFormat);
    videoCapture->StartCapture();
    videoCapture->GetImg(buffer, tv);

    //到这里buffer里存的是jpeg数据

    //要的数据
    MyVideoCapture::OutInfo outInfo;
    Mat img;
    img.create(height, width, CV_8UC3);
    memcpy(img.data, buffer.data(), buffer.size());
    if (IMGSHOW){
        Mat imgShow;
        cvtColor(img,imgShow,COLOR_YUV2BGRA_YUYV);
        imshow("imgShow", img);
        waitKey(0);
    }

    MyVideoCapture::GetOutInfo(outInfo, tv, pixelFormat, width, height, (char *) buffer.data(), buffer.size());

    buffer.clear();

    videoCapture->Close();
    delete videoCapture;

}