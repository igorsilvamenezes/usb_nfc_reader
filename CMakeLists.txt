cmake_minimum_required(VERSION 3.5)

project(nfc_reader LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable( nfc_reader
                nfc_reader.c
                nfc_device.c
                usb_device.c
                nfc_utils.c )

include_directories(.)

target_link_libraries(nfc_reader usb)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")