cmake_minimum_required(VERSION 3.15)
project(duckdb_grpc_wrapper)

set(CMAKE_CXX_STANDARD 17)

# Dependencies
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf REQUIRED)

# DuckDB from Local Source Build
set(DUCKDB_ROOT "/Users/zeyang/Desktop/growth/duckdb")
set(DUCKDB_BUILD "${DUCKDB_ROOT}/build/release")

find_path(DUCKDB_INCLUDE_DIR duckdb.hpp PATHS "${DUCKDB_ROOT}/src/include" NO_DEFAULT_PATH)
find_library(DUCKDB_LIBRARY duckdb PATHS "${DUCKDB_BUILD}/src" NO_DEFAULT_PATH)

if(NOT DUCKDB_INCLUDE_DIR OR NOT DUCKDB_LIBRARY)
    message(FATAL_ERROR "Local DuckDB build not found. Please run 'make' in ${DUCKDB_ROOT}")
endif()

include_directories(${DUCKDB_INCLUDE_DIR})

# Protobuf Generation
set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)
set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})

find_program(PROTOC_EXECUTABLE protoc)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

set(PROTO_FILES ${PROTO_SRC_DIR}/database.proto)

add_custom_command(
    OUTPUT "${PROTO_BINARY_DIR}/database.pb.cc" "${PROTO_BINARY_DIR}/database.pb.h" "${PROTO_BINARY_DIR}/database.grpc.pb.cc" "${PROTO_BINARY_DIR}/database.grpc.pb.h"
    COMMAND ${PROTOC_EXECUTABLE}
    ARGS --grpc_out="${PROTO_BINARY_DIR}" --cpp_out="${PROTO_BINARY_DIR}" --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_EXECUTABLE}" -I "${PROTO_SRC_DIR}" "${PROTO_FILES}"
    DEPENDS "${PROTO_FILES}"
)

add_library(grpc_proto_lib
    "${PROTO_BINARY_DIR}/database.pb.cc"
    "${PROTO_BINARY_DIR}/database.pb.h"
    "${PROTO_BINARY_DIR}/database.grpc.pb.cc"
    "${PROTO_BINARY_DIR}/database.grpc.pb.h"
)
target_link_libraries(grpc_proto_lib gRPC::grpc++ protobuf::libprotobuf)
target_include_directories(grpc_proto_lib PUBLIC "${PROTO_BINARY_DIR}")

# Server
add_executable(duckdb_server src/server.cpp)
target_link_libraries(duckdb_server grpc_proto_lib ${DUCKDB_LIBRARY} gRPC::grpc++ protobuf::libprotobuf)

# Client
add_executable(duckdb_client src/client.cpp)
target_link_libraries(duckdb_client grpc_proto_lib gRPC::grpc++ protobuf::libprotobuf)
