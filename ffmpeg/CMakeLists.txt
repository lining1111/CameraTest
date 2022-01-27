set(FFMPEG_LIBS avcodec avformat avutil swresample swscale avfilter)
#ffmpeg1
add_executable(ffmpeg1 ../ffmpeg/ffmpeg1.cpp)
target_link_libraries(ffmpeg1 ${FFMPEG_LIBS} ${LINK_LIBS} )