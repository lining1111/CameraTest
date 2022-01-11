//
// Created by lining on 12/22/21.
//

#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Radar_ARS408.h"

using namespace std;

Radar_ARS408::Radar_ARS408(string ip, uint16_t port) {

}

Radar_ARS408::~Radar_ARS408() {
    if (isOpen){
        Close();
    }
}

int Radar_ARS408::Open() {
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

int Radar_ARS408::Close() {
    if (isOpen){
        close(fd);
    }
    isOpen = false;
    return 0;
}

int Radar_ARS408::SetMode(Mode mode) {




    return 0;
}
