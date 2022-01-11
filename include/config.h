//
// Created by lining on 1/5/22.
//

#ifndef _CONFIG_H
#define _CONFIG_H

#include <opencv2/opencv.hpp>

typedef struct {
    double x;
    double y;
} point_t;

typedef struct
{
    int width;
    int height;
    cv::Mat first_pic_mat;//头帧Mat
    int **calibration_x_table;//相机标定用x坐标表
    int **calibration_y_table;//相机标定用y坐标表
    cv::Rect matchBox;//特征点识别框
    float x_drift;//抖动位移量x方向
    float y_drift;//抖动位移量y方向
    float x_vibrate_max;//抖动位移量x方向阈值
    float y_vibrate_max;//抖动位移量y方向阈值
    double perspective_trans_fa[3][3];//透视变换用系数矩阵
    double H;//透视变换图像高度
    double x_distance;//透视变换最左边的直线与Y轴之间的实际测量距离
    double y_distance;//透视变换最下边的直线与X轴之间的实际测量距离
    double radar_x;//雷达偏移量x
    double radar_y;//雷达偏移量y
    double radar_theta;//校正参数
    int orientations;//相机朝向 东：1  南：2   西：3   北：4
    double deltax_south;//转换为路口坐标系的偏移量x
    double deltay_south;//转换为路口坐标系的偏移量y
    double deltax_north;//转换为路口坐标系的偏移量x
    double deltay_north;//转换为路口坐标系的偏移量y
    double deltax_west;//转换为路口坐标系的偏移量x
    double deltay_west;//转换为路口坐标系的偏移量y
    double deltax_east;//转换为路口坐标系的偏移量x
    double deltay_east;//转换为路口坐标系的偏移量y
    int flag_south;//十字路口设定标志位，1为有这个方向的路 0为没有这个方向的路
    int flag_west;
    int flag_north;
    int flag_east;
    double widthX;//融合前去除重合车辆用偏移量x
    double widthY;//融合前去除重合车辆用偏移量y
    point_t real_left_point;
    point_t real_right_point;
    point_t real_top_point;
    point_t real_bottom_point;
    point_t pixel_left_point;
    point_t pixel_right_point;
    point_t pixel_top_point;
    point_t pixel_bottom_point;
    double MPPW;//宽度米像素
    double MPPH;//高度米像素
    double threshold_value;//门限值
}BASE_INFO;


extern BASE_INFO base_info;

//从配置文件中读转换真实坐标的参数
int read_real_loc_info(char *filename);


#endif //_CONFIG_H
