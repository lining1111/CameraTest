#VideoCaptureTest
add_executable(videoCaptureTest videoCaptureTest.cpp
        ../src/MyVideoCapture.cpp
        ../src/MyVideoCaptureUSB.cpp)
target_link_libraries(videoCaptureTest ${OpenCV_LIBS} ${LINK_LIBS})

#configTest
add_executable(configTest configTest.cpp
        ../src/config.cpp)
target_link_libraries(configTest ${OpenCV_LIBS} ${LINK_LIBS})