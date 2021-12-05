cmake_minimum_required(VERSION 3.10.2)
project("raptorfft")
set(installhelper_version 0.1.0)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(raptordsp CONFIG REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)

add_library(raptorfft SHARED "src/fft_preview.cpp" "src/fft_preview_client.cpp" "include/raptorfft/fft_preview.h" "include/raptorfft/fft_preview_client.h")

target_include_directories(raptorfft PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(raptorfft raptordsp Volk::volk FFTW3::fftw3f)

# Set up install
set(installhelper_name raptorfft)
include(install_helper.cmake)