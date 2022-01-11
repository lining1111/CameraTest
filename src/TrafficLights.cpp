//
// Created by lining on 12/22/21.
//

#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TrafficLights.h"

TrafficLights::TrafficLights(string ip, uint16_t port) {
    this->ip = ip;
    this->port = port;
}

TrafficLights::~TrafficLights() {
    if (isOpen) {
        Close();
    }
}

int TrafficLights::Open() {

    //tcp socket
    int sock = 0;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        printf("%s\n", strerror(errno));
        return -1;
    }
    struct timeval timeout = {
            .tv_sec=10
    };

    int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof(struct timeval));
    if (ret == -1) {
        printf("%s\n", strerror(errno));
    }
    ret = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *) &timeout, sizeof(struct timeval));
    if (ret == -1) {
        printf("%s\n", strerror(errno));
    }

    struct sockaddr_in dstaddr;
    memset(&dstaddr, 0, sizeof(struct sockaddr_in));
    dstaddr.sin_family = AF_INET;
    dstaddr.sin_port = htons(port);
    //ret=inet_aton(ip,&myaddr.sin_addr);
    dstaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (dstaddr.sin_addr.s_addr == -1) {
        printf("%s\n", strerror(errno));
        return -1;
    }

    int reuseport = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuseport, sizeof(reuseport));
    if (ret == -1) {
        printf("%s\n", strerror(errno));
    }

    ret = connect(sock, (struct sockaddr *) &dstaddr, sizeof(struct sockaddr_in));
    if (ret != 0) {
        printf("%s\n", strerror(errno));
        return -1;
    }

    fd = sock;

    isOpen = true;

    return 0;
}

int TrafficLights::Close() {
    if (isOpen) {
        close(fd);
    }
    isOpen = false;

    return 0;
}

int TrafficLights::GetStatus(TrafficLights::Status &status) {
    if (!isOpen) {
        return -1;
    }



    return 0;
}
