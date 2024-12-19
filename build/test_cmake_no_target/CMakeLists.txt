cmake_minimum_required(VERSION 3.9.0)
project(test)

find_package(Tiff CONFIG REQUIRED)

add_executable(test test.c)
target_include_directories(test PRIVATE ${TIFF_INCLUDE_DIR})
set_target_properties(test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(test PRIVATE ${TIFF_LIBRARIES})
