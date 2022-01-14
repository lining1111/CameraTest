#VideoCaptureTest
add_executable(videoCaptureTest videoCaptureTest.cpp
        ../src/MyVideoCapture.cpp
        ../src/MyVideoCaptureUSB.cpp)
target_link_libraries(videoCaptureTest ${OpenCV_LIBS} ${LINK_LIBS})

#configTest
add_executable(configTest configTest.cpp
        ../src/config.cpp)
target_link_libraries(configTest ${OpenCV_LIBS} ${LINK_LIBS})

#rtspTest
add_executable(rtspTest rtspTest.cpp
        ../src/RTSPH264.cpp)
target_link_libraries(rtspTest ${OpenCV_LIBS} ${LIVE555_LIBS} ${LINK_LIBS})

#testH264VideoStreamer
add_executable(testH264VideoStreamer testH264VideoStreamer.cpp)
target_link_libraries(testH264VideoStreamer ${OpenCV_LIBS} ${LIVE555_LIBS} ${LINK_LIBS})

#testMyH264VideoStreamer
add_executable(testMyH264VideoStreamer testMyH264VideoStreamer.cpp)
target_link_libraries(testMyH264VideoStreamer ${OpenCV_LIBS} ${LIVE555_LIBS} ${LINK_LIBS})
