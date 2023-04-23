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
    src/Tool/WIP.cpp
)

add_library(Json STATIC
    src/Tool/lib_json/json_reader.cpp
    src/Tool/lib_json/json_value.cpp
    src/Tool/lib_json/json_writer.cpp
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
    Json
    CommonUtilLib
    ProtoUtilLib
)

add_executable(CenterServer_jk
    $<TARGET_OBJECTS:BaseObj>
    src/Source/CenterServer.cpp
)
target_link_libraries(CenterServer_jk  ${libraries_to_link})

add_executable(DBServer_jk
    $<TARGET_OBJECTS:BaseObj>
    src/Source/DBServer.cpp
)
target_link_libraries(DBServer_jk  ${libraries_to_link})

add_executable(GateServer_jk
    $<TARGET_OBJECTS:BaseObj>
    src/Source/GateServer.cpp
)
target_link_libraries(GateServer_jk  ${libraries_to_link})

add_executable(LogicServer_jk
    $<TARGET_OBJECTS:BaseObj>
    src/Source/LogicServer.cpp
    src/Logic/Room.cpp
    src/Logic/Room.h
)
target_link_libraries(LogicServer_jk  ${libraries_to_link})


