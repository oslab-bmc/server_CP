set(
    OUTPUT#아래 코드를 OUTPUT 변수로 묶음
    server_CP
)

## Source file
set(
    SOURCE_FILES #아래 코드들 SOURCE_FILES 변수로 묶음.
    server_CP.cpp
    process_reading.cpp
    handler.cpp
    sys_reading.cpp
    signal_reading.cpp

)

set(ROOT_DIR /home/oslab/KETI_BMC)
set(PROJECT_DIR ${ROOT_DIR}/server_CP/source)
set(PROJECT_INC ${PROJECT_DIR}/include)
set(JSON_DIR ${PROJECT_DIR}/jsoncpp)
#라이브러리 경로
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSROOT ${ROOT_DIR}/SDK/sysroots/armv7ahf-vfpv4d16-KETI-linux-gnueabi)
set(CROSS_COMPILE ${ROOT_DIR}/SDK/sysroots/x86_64-oesdk-linux/usr/bin/arm-KETI-linux-gnueabi/arm-KETI-linux-gnueabi-)
set(GPP ${CROSS_COMPILE}g++)
set(CMAKE_CXX_COMPILER ${GPP})
set(CMAKE_CXX_FLAGS "-lcrypto -lssl -lcpprest -pthread -std=c++17 -march=armv7-a -mfpu=vfpv4-d16 -mfloat-abi=hard")
message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_DIR}/output/bin)
include_directories(
    ${PROJECT_INC}
)

add_executable(${OUTPUT}  ${SOURCE_FILES})

target_link_libraries( ${OUTPUT}
    ${PROJECT_DIR}/output/lib/libjsoncpp.a
)
