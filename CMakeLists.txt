cmake_minimum_required(VERSION 3.16)
project(homework-network)

set(CMAKE_CXX_STANDARD 14)
link_directories(/usr/local/lib)

include_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/logic)
include_directories(${PROJECT_SOURCE_DIR}/protobuf/proto_out_c)
include_directories(${PROJECT_SOURCE_DIR}/protobuf)
include_directories(${PROJECT_SOURCE_DIR}/mysqllib)
include_directories(${PROJECT_SOURCE_DIR}/model)
include_directories(${PROJECT_SOURCE_DIR}/network)
aux_source_directory(${PROJECT_SOURCE_DIR}/logic logic)
aux_source_directory(${PROJECT_SOURCE_DIR}/protobuf/proto_out_c proto)
aux_source_directory(${PROJECT_SOURCE_DIR}/mysqllib mysqllib)
aux_source_directory(${PROJECT_SOURCE_DIR}/model model)
aux_source_directory(${PROJECT_SOURCE_DIR}/network network)

add_executable(client client.cpp ${network} ${proto})
target_link_libraries(client protobuf)

add_executable(logic_server logic_server.cpp ${network} ${proto} ${logic})
target_link_libraries(logic_server protobuf)

add_executable(dbserver dbserver.cpp ${network} ${mysqllib} ${proto})
target_link_libraries(dbserver mysqlclient protobuf hiredis)

add_executable(gate_server gate_server.cpp ${network} ${proto})
target_link_libraries(gate_server protobuf)

add_compile_options(PUBLIC -g)
