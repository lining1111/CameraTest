# 摄像头设置、采集、rtsp相关

## v4l2 https://blog.csdn.net/weixin_30373263/article/details/117347381

    v4l2-ctl -all 列出所有摄像头信息
    v4l2-ctl --list-devices

## live555 http://www.live555.com/  live555文档：http://www.live555.com/liveMedia/

vlc观看有延迟，主要是vlc设置问题 工具---偏好设置---(显示全部)输入\编解码器---网络缓存(ms)

代码中实现 rtsp推摄像头的h264流， Camera FramedSourceH264 OnDemandServerMediaSubsession RTSPH264 主要是靠2个线程：

1.数据采集线程：摄像头采集YUYV数据，x264编码成h264格式，放入fifo内

2.数据推流线程：开启rtsp server 添加自定义的mediaSubSession 这个subsession会新建一个自定义的FramedSource 自定义的好处，就是分解业务，后期对照live555的说明文档进行优化
OnDemandServerMediaSubsessionH264 和 liveMedia下的H264VideoFileServerMediaSubsession.cpp类似
FramedSourceH264就是FramedSource的子类，重点是实现虚函数 doGetNextFrame