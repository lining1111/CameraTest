//
// Created by lining on 1/14/22.
//

/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********/
// Copyright (c) 1996-2022, Live Networks, Inc.  All rights reserved
// A test program that reads a H.264 Elementary Stream video file
// and streams it using RTP
// main program
//
// NOTE: For this application to work, the H.264 Elementary Stream video file *must* contain SPS and PPS NAL units,
// ideally at or near the start of the file.  These SPS and PPS NAL units are used to specify 'configuration' information
// that is set in the output stream's SDP description (by the RTSP server that is built in to this application).
// Note also that - unlike some other "*Streamer" demo applications - the resulting stream can be received only using a
// RTSP client (such as "openRTSP")

#include <liveMedia.hh>

#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <thread>
#include <iostream>

using namespace std;

static void Thread(void *p);

void play(void *p); // forward
void announceURL(RTSPServer *rtspServer, ServerMediaSession *sms);

typedef struct {
    TaskScheduler *scheduler;
    UsageEnvironment *env;
    char const *inputFileName = "./video/test.264";
//    char const *inputFileName = "./video_fifo";
    H264VideoStreamFramer *videoSource;
    RTPSink *videoSink;
    RTCPInstance *rtcp;
} My;

int main(int argc, char **argv) {
    My my;
//    thread thread1;
//    thread1 = thread(Thread, &my);
//    thread1.detach();
    //开启server子进程
//    if (fork() > 0) {
//        Thread(&my);
//        cout<<"子进程"<<endl;
//    }else{
////        Thread(&my);
//    }
    Thread(&my);
    while (true) {
//        sleep(1);
    }

}


static void Thread(void *p) {
    if (p == nullptr) {
        return;
    }
    auto my = (My *) p;

    cout << "rtsp server start" << endl;

    string stream;
    stream = "./video/test.264";



    //1 env
    my->scheduler = BasicTaskScheduler::createNew();
    my->env = BasicUsageEnvironment::createNew(*my->scheduler);

    // Create 'groupsocks' for RTP and RTCP:
    struct sockaddr_storage destinationAddress;
    destinationAddress.ss_family = AF_INET;
    ((struct sockaddr_in &) destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*my->env);
    // Note: This is a multicast address.  If you wish instead to stream
    // using unicast, then you should use the "testOnDemandRTSPServer"
    // test program - not this test program - as a model.
    const unsigned short rtpPortNum = 18888;
    const unsigned short rtcpPortNum = rtpPortNum + 1;
    const unsigned char ttl = 255;

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);

    Groupsock rtpGroupsock(*my->env, destinationAddress, rtpPort, ttl);
    rtpGroupsock.multicastSendOnly(); // we're a SSM source
    Groupsock rtcpGroupsock(*my->env, destinationAddress, rtcpPort, ttl);
    rtcpGroupsock.multicastSendOnly(); // we're a SSM source

    // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
    OutPacketBuffer::maxSize = 100000;
    my->videoSink = H264VideoRTPSink::createNew(*my->env, &rtpGroupsock, 96);
    // Create (and start) a 'RTCP instance' for this RTP sink:
    const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen + 1];
    gethostname((char *) CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    my->rtcp = RTCPInstance::createNew(*my->env, &rtcpGroupsock,
                                       estimatedSessionBandwidth, CNAME,
                                       my->videoSink, NULL /* we're a server */,
                                       True /* we're a SSM source */);

    auto env = my->env;


    // Note: This starts RTCP running automatically
    UserAuthenticationDatabase *authDB = nullptr;

    RTSPServer *rtspServer = RTSPServer::createNew(*env, 8554, authDB);
    if (rtspServer == nullptr) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

    ServerMediaSession *sms = ServerMediaSession::createNew(*env, "testStreamer",
                                                            stream.c_str(),
                                                            "Session streamed by \"testH264VideoStreamer\"",
                                                            True /*SSM*/);
    sms->addSubsession(PassiveServerMediaSubsession::createNew(*my->videoSink,
                                                               my->rtcp));
    rtspServer->addServerMediaSession(sms);
    announceURL(rtspServer, sms);

    // Start the streaming:
    *env << "Beginning streaming...\n";
    play(my);

    env->taskScheduler().doEventLoop(); // does not return

}

void afterPlaying(void *clientData) {
    auto my = (My *) clientData;

    *my->env << "...done reading from file\n";
    my->videoSink->stopPlaying();
    Medium::close(my->videoSource);
    // Note that this also closes the input file that this source read from.

    // Start playing once again:
    play(my);
}

void play(void *p) {
    auto my = (My *) p;
    // Open the input file as a 'byte-stream file source':
    ByteStreamFileSource *fileSource
            = ByteStreamFileSource::createNew(*my->env, my->inputFileName);
    if (fileSource == NULL) {
        *my->env << "Unable to open file \"" << my->inputFileName
                 << "\" as a byte-stream file source\n";
        exit(1);
    }

    FramedSource *videoES = fileSource;

    // Create a framer for the Video Elementary Stream:
    my->videoSource = H264VideoStreamFramer::createNew(*my->env, videoES);

    // Finally, start playing:
    *my->env << "Beginning to read from file...\n";
    my->videoSink->startPlaying(*my->videoSource, afterPlaying, my->videoSink);
}

void announceURL(RTSPServer *rtspServer, ServerMediaSession *sms) {
    if (rtspServer == NULL || sms == NULL) return; // sanuty check

    UsageEnvironment &env = rtspServer->envir();

    env << "Play this stream using the URL ";
    if (weHaveAnIPv4Address(env)) {
        char *url = rtspServer->ipv4rtspURL(sms);
        env << "\"" << url << "\"";
        delete[] url;
        if (weHaveAnIPv6Address(env)) env << " or ";
    }
    if (weHaveAnIPv6Address(env)) {
        char *url = rtspServer->ipv6rtspURL(sms);
        env << "\"" << url << "\"";
        delete[] url;
    }
    env << "\n";
}