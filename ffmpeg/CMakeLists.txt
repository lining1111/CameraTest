set(FFMPEG_LIBS avcodec avformat)
#ffmpeg1
add_executable(ffmpeg1 ../ffmpeg/ffmpeg1.cpp)
target_link_libraries(ffmpeg1 ${OpenCV_LIBS} ${LIVE555_LIBS} ${LINK_LIBS} ${FFMPEG_LIBS})