//
// Created by lining on 1/26/22.
//

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
    FILE *pFile;
    char szFileName[32];
    int y;
    //Open file
    sprintf(szFileName, "frame%d.ppm", iFrame);
    pFile = fopen(szFileName, "wb");

    if (pFile == nullptr) {
        return;
    }
    //write ppm header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);
    //write pixel data
    for (y = 0; y < height; y++) {
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);
    }
    //close file
    fclose(pFile);
}

int main(int argc, char *argv[]) {
    AVFormatContext *pFormatCtx;
    int i, videoStream;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame;
    AVFrame *pFrameRGB;
    AVPacket packet;
    int frameFinished;
    int numBytes;
    uint8_t *buffer;

    if (argc < 2) {
        printf("please provide a movie file\n");
        return -1;
    }
    //register all formats and codes
    av_register_all();
    //support network stream input
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    //Open the media file and read the header
    if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0) {
        printf("couldn't open file \n");
        return -1;
    }
    //retrieve stream information
//    AVDictionary *options;
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
        return -1;
    //dump information about file into standard error
    av_dump_format(pFormatCtx, -1, argv[1], 0);
    // Find the first video stream
    videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1)
        return -1;
    //get a pointer to the codec context for the video stream
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        fprintf(stderr, "unsupported codec \n");
        return -1;
    }
    //open codec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
        return -1;
    //allocate video frame
    pFrame = av_frame_alloc();
    if (NULL == pFrame)
        return -1;
    //allocate an avframe structure
    pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL)
        return -1;
    //determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    //buffer = (uint8_t *)av_malloc_attrib(numBytes * sizeof(uint8_t));
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    i = 0;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            if (frameFinished) {
                struct SwsContext *img_convert_ctx = NULL;
                img_convert_ctx = sws_getCachedContext(img_convert_ctx, pCodecCtx->width, pCodecCtx->height, \
                    pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL,
                                                       NULL, NULL);
                if (!img_convert_ctx) {
                    fprintf(stderr, "Cannot initialize sws conversion context\n");
                    exit(1);
                }
                /* int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
                  const int srcStride[], int srcSliceY, int srcSliceH,
                  uint8_t *const dst[], const int dstStride[]); */
                sws_scale(img_convert_ctx, pFrame->data, \
                pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                if (i++ < 50)
                    SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
            }
        }
        av_free_packet(&packet);
    }
    //free the RGB image
    av_free(buffer);
    av_free(pFrameRGB);
    av_free(pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}