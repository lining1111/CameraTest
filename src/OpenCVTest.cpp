//
// Created by lining on 12/14/21.
//

#include "OpenCVTest.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void OpenCVTest(void) {
    VideoCapture capture(0);
    if (capture.isOpened()) {
        cout << "success" << endl;
    }else{
        return;
    }
    capture.set(CAP_PROP_SETTINGS,1);
    //设置摄像头参数
    capture.set(CAP_PROP_FRAME_WIDTH,640);//宽度
    capture.set(CAP_PROP_FRAME_HEIGHT, 480);//高度
    capture.set(CAP_PROP_FPS, 30);//帧数
//    capture.set(CAP_PROP_BRIGHTNESS, 1);//亮度
//    capture.set(CAP_PROP_CONTRAST, 40);//对比度
//    capture.set(CAP_PROP_SATURATION,50);//饱和度
//    capture.set(CAP_PROP_HUE,50);//色调
//    capture.set(CAP_PROP_EXPOSURE,50);//曝光

    //打印摄像头参数
    printf("width = %.2f\n",capture.get(CAP_PROP_FRAME_WIDTH));
    printf("height = %.2f\n",capture.get(CAP_PROP_FRAME_HEIGHT));
    printf("fbs = %.2f\n",capture.get(CAP_PROP_FPS));
    printf("brightness = %.2f\n",capture.get(CAP_PROP_BRIGHTNESS));
    printf("contrast = %.2f\n",capture.get(CAP_PROP_CONTRAST));
    printf("saturation = %.2f\n",capture.get(CAP_PROP_SATURATION));
    printf("hue = %.2f\n",capture.get(CAP_PROP_HUE));
    printf("exposure = %.2f\n",capture.get(CAP_PROP_EXPOSURE));
    printf("focus = %.2f\n",capture.get(CAP_PROP_FOCUS));

    Mat frame;
    while (capture.isOpened()){
        capture>>frame;
        if (!frame.empty()){
            imshow("capture",frame);
        }else{
            cout<<"can not get"<<endl;
        }

        if (waitKey(10) == 27){
            break;
        }
    }
}
