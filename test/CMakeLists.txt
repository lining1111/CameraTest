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