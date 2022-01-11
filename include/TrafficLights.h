//
// Created by lining on 12/22/21.
//

#ifndef _TRAFFICLIGHTS_H
#define _TRAFFICLIGHTS_H

#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 交通信号灯
 */

using namespace std;

class TrafficLights {

    typedef struct {
        int id;
        int green;
        int yellow;
        int red;
    } LightStatus;

    typedef struct {
        int phaseID;
        int greenRemainingTime;
        int yellowRemainingTime;
        int redRemainingTime;
        char currentTime[20];
        vector<LightStatus> vector_lightStatus;
    } Status;


public:
    int fd;
    string ip;
    uint16_t port;
public:
    bool isOpen = false;
public:
    TrafficLights(string ip, uint16_t port);

    ~TrafficLights();

public:
    /**
     * 打开信号灯控制设备
     * @return 0:success -1:fail
     */
    int Open();

    /**
     * 关闭信号灯控制设备
     * @return 0:success -1:fail
     */
    int Close();

    /**
     * 获取信号灯状态
     * @param status
     * @return 0:success -1:fail
     * @brief
     * send 0xF0,0x00,0x07,0x05,0x7E crc
     * recv 0xF0,0x00,0x1C,0x05,0x7E,0x01,0x01,0x00,0x00,0x03,0x5D,0xB7,0x6D,0xDB,0xB6,0x6D,0xDB,0xB6,0x6D,0x01,0x15,0x0B,0x05,0x0E,0x24,0x25,0x29,0x07
     */
    int GetStatus(Status &status);


};

#ifdef __cplusplus
}
#endif

#endif //_TRAFFICLIGHTS_H
