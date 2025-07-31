# CMakeList.txt : CMake project for NTSIL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("NTSIL")

# Add source to this project's executable.
add_executable (NTSIL "main.cpp" "master.h")

# TODO: Add tests and install targets if needed.
target_include_directories(NTSIL PUBLIC "include")

# TODO: Add tests and install targets if needed.
target_include_directories(NTSIL PUBLIC "include")