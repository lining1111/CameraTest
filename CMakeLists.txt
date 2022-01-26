##cmake_minimum_required(VERSION 3.20)
project(CameraTest)

set(TargetName "CameraTest")
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

set(LINK_LIBS m pthread jpeg iconv sqlite3 ssl crypto x264)
set(LIVE555_LIBS liveMedia groupsock BasicUsageEnvironment UsageEnvironment)
include_directories(.)
include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})
#live555
include_directories(/usr/local/include/BasicUsageEnvironment)
include_directories(/usr/local/include/groupsock)
include_directories(/usr/local/include/liveMedia)
include_directories(/usr/local/include/UsageEnvironment)


set(SRC_DIRS src
        )

foreach (src_dir ${SRC_DIRS})
    aux_source_directory(${src_dir} src_dir_file)
    set(SRC_FILES ${SRC_FILES} ${src_dir_file})
endforeach (src_dir)

add_executable(${TargetName} main.cpp ${SRC_FILES})
target_link_libraries(${TargetName} ${OpenCV_LIBS} ${LIVE555_LIBS} ${LINK_LIBS})

##test
add_subdirectory(test)
add_subdirectory(ffmpeg)
