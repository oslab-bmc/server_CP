set(
    OUTPUT#아래 코드를 OUTPUT 변수로 묶음
    server_CP
)

## Source file
set(
    SOURCE_FILES #아래 코드들 SOURCE_FILES 변수로 묶음.
    server_CP.cpp
)

set(ROOT_DIR /home/keti/KETI_BMC)
set(PROJECT_DIR ${ROOT_DIR}/server_CP/source)
set(JSON_INC ${PROJECT_DIR}/jsoncpp)
#라이브러리 경로
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CROSS_COMPILE ${ROOT_DIR}/SDK/sysroots/x86_64-oesdk-linux/usr/bin/arm-KETI-linux-gnueabi/arm-KETI-linux-gnueabi-)
set(GCC ${CROSS_COMPILE}gcc)
set(GPP ${CROSS_COMPILE}g++)
set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GPP})
set(CMAKE_CXX_FLAGS "-lboost_system -lcrypto -lssl -lcpprest -pthread -std=c++17")
message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")

include_directories(
    ${JSON_INC}
)

# # Source
# link_directories(
#     ${DBUS_LIBRARY_DIR}
# )

add_executable(${OUTPUT}  ${SOURCE_FILES})

# target_link_libraries( ${OUTPUT}
    
# )