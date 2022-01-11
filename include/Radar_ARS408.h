//
// Created by lining on 12/22/21.
//

#ifndef _RADAR_ARS408_H
#define _RADAR_ARS408_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string>

using namespace std;

class Radar_ARS408 {

    enum Mode {
        OUTPUT_REALTIME_MODE = 1, //只输出实时（含排队）模式；
        OUTPUT_STATISTICS_MODE, //只输出统计模式
        SILENT_WAIT_MODE, //静默等待，无数据输出
        REALTIME_DATA_OUTPUT_MODE, //实时（含排队）/统计同时输出
        RAW_DATA_OUTPUT_MODE, //原始数据输出模式
        OUTPUT_SECTION_TRIGGER_MODE //只输出断面触发模式
    };


public:
    int fd;
    string ip;
    uint16_t port;

    bool isOpen = false;
public:
    Radar_ARS408(string ip, uint16_t port);

    ~Radar_ARS408();

public:
    int Open();

    int Close();

    int SetMode(Mode mode);

};


#ifdef __cplusplus
}
#endif

#endif //_RADAR_ARS408_H
