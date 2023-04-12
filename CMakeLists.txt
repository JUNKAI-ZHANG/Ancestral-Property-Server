cmake_minimum_required(VERSION 3.3.2)

set(CMAKE_CXX_STANDARD 11)

# 设置静态库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置动态库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dll)

project(Ancestral-Property-Server)

# ADD DLL-PATH
link_directories(/usr/lib64/mysql)

add_library(ProtoUtilLib STATIC
    src/Protobuf/Login.pb.cc
    src/Protobuf/Server.pb.cc
    src/Protobuf/Frame.pb.cc
    src/Protobuf/Room.pb.cc
    src/Tool/Message.cpp
)

add_library(CommonUtilLib STATIC
    src/Tool/EpollMgr.cpp
    src/Tool/Event.cpp
    src/Tool/RingBuffer.cpp
    src/Tool/Timer.cpp
    src/Tool/Room.cpp
)

add_library(BaseObj OBJECT
    src/Source/ServerBase.cpp
    src/Source/FuncServer.cpp
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
    src/Source/CenterServer.cpp
)
add_compile_options(CenterServer -g)
target_link_libraries(CenterServer  ${libraries_to_link})

add_executable(DBServer
    $<TARGET_OBJECTS:BaseObj>
    src/Source/DBServer.cpp
)
add_compile_options(DBServer -g)
target_link_libraries(DBServer  ${libraries_to_link})

add_executable(GateServer
    $<TARGET_OBJECTS:BaseObj>
    src/Source/GateServer.cpp
)
add_compile_options(GateServer -g)
target_link_libraries(GateServer  ${libraries_to_link})

add_executable(LogicServer
    $<TARGET_OBJECTS:BaseObj>
    src/Source/LogicServer.cpp
)
add_compile_options(LogicServer -g)
target_link_libraries(LogicServer  ${libraries_to_link})


