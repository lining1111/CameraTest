//
// Created by lining on 12/21/21.
//

#ifndef _MYVIDEOCAPTURE_H
#define _MYVIDEOCAPTURE_H

#include <string>
#include <linux/videodev2.h>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


/**
 * 摄像头基类
 */

#ifdef __cplusplus
extern "C" {
#endif

class MyVideoCapture {
public:
    //摄像头连接方式：网络、USB
    enum VideoConMode {
        VCM_Unknown = 0,
        VCM_Net,
        VCM_USB
    };

    //分辨率
    typedef struct {
        uint32_t width;//宽
        uint32_t height;//高
    } Resolution;

    //摄像头能力
    typedef struct {
        uint32_t width;//图像宽度
        uint32_t height;//图像高度
        vector<uint32_t> supportFormat;//支持的图片格式
        uint32_t pixelFormat;//图片像素格式
        uint32_t fps;//帧数
    } Capability;

    typedef struct {
        int status;//0 1
        int height;
        int width;
//        char *rgbData;
//        int rgbSize;
        char *jpgData;
        int jpgSize;
        struct timeval recordTime;
    } OutInfo;


public:
    int fd = 0;//文件操作符
    string name;//摄像头名称
    VideoConMode videoConMode;//摄像头连接方式
    Capability capability;//相机的能力
public:
    //相机状态
    bool isOpen = false;//是否打开
    bool isCapture = false;//是否开启获取图片
public:
    MyVideoCapture();

    ~MyVideoCapture();

    static int
    GetOutInfo(MyVideoCapture::OutInfo &outInfo, struct timeval tv, uint32_t pixelFormat, uint32_t cols,
               int rows, char *buf, int bufSize);

public:
    /**
     * 打开相机
     * @return 0：成功 -1：失败
     */
    virtual int Open() = 0;

    /**
     * 关闭相机
     * @return 0：成功 -1：失败
     */
    virtual int Close() = 0;

    /**
     * 获取相机像素宽和高
     * @param width 宽度
     * @param height 高度
     * @return 0：成功 -1：失败
     */
    virtual int GetPixelWidthHeight(uint32_t &width, uint32_t &height) = 0;

    /**
     * 设置相机像素的宽度和高度
     * @param width 宽度
     * @param height 高度
     * @return 0：成功 -1：失败
     */
    virtual int SetPixelWidthHeight(uint32_t width, uint32_t height) = 0;

    /**
     * 获取相机支持的图片格式
     * @param supportFormat 支持的图片格式数组
     * @return 0：成功 -1：失败
     */
    virtual int GetSupportFormat(vector<uint32_t> &supportFormat) = 0;

    /**
     * 获取当前相机的图片格式
     * @param pixelFormat 格式
     * @return 0：成功 -1：失败
     */
    virtual int GetCurFormat(uint32_t &pixelFormat) = 0;

    /**
     * 设置当前相机的图片格式
     * @param pixelFormat 格式
     * @return 0：成功 -1：失败
     */
    virtual int SetCurFormat(uint32_t pixelFormat) = 0;

    /**
     * 获取相机的帧率
     * @param fps 帧率
     * @return 0：成功 -1：失败
     */
    virtual int GetFps(uint32_t &fps) = 0;

    /**
     * 设置相机的帧率
     * @param fps 帧率
     * @return 0：成功 -1：失败
     */
    virtual int SetFps(uint32_t fps) = 0;

    /**
     * 开启图片获取
     * @return 0：成功 -1：失败
     */
    virtual int StartCapture(void) = 0;

    /**
     * 获取一张图片
     * @param data
     * @param len
     * @param tv
     * @return
     */
    virtual int GetImg(vector<unsigned char> &vector_data, timeval &tv) = 0;

    /**
     * 关闭图片获取
     * @return 0：成功 -1：失败
     */
    virtual int StopCapture(void) = 0;

};

#ifdef __cplusplus
}
#endif


#endif //_MYVIDEOCAPTURE_H
