cmake_minimum_required(VERSION 3.3.2)

set(CMAKE_CXX_STANDARD 11)

# 设置静态库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置动态库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dll)

project(Holy)

# ADD DLL-PATH
link_directories(/usr/lib64/mysql)

add_library(ProtoUtilLib STATIC
    ./Protobuf/NetworkData.pb.cc
    ./Tool/ProtoUtil.cpp
)

add_library(CommonUtilLib STATIC
    ./Tool/EpollMgr.cpp 
    ./Tool/Event.cpp 
    ./Tool/RingBuffer.cpp 
    ./Tool/Timer.cpp 
)

add_library(BaseObj OBJECT
    ./Source/ServerBase.cpp
    ./Source/FuncServer.cpp
)

set(libraries_to_link
    protobuf
    pthread
    mysqlclient
    hiredis
    CommonUtilLib
    ProtoUtilLib
)

# include_directories(./Header)

add_executable(CenterServer
    $<TARGET_OBJECTS:BaseObj>
    ./Source/CenterServer.cpp
)
target_link_libraries(CenterServer  ${libraries_to_link})

add_executable(DBServer
    $<TARGET_OBJECTS:BaseObj>
    ./Source/DBServer.cpp
)
target_link_libraries(DBServer  ${libraries_to_link})

add_executable(GateServer
    $<TARGET_OBJECTS:BaseObj>
    ./Source/GateServer.cpp 
)
target_link_libraries(GateServer  ${libraries_to_link})

add_executable(LogicServer
    $<TARGET_OBJECTS:BaseObj>
    ./Source/LogicServer.cpp
)
target_link_libraries(LogicServer  ${libraries_to_link})
