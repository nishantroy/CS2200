cmake_minimum_required(VERSION 3.2)
project(cachesim)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99 -pedantic ")

add_executable(cachesim cachesim.c cachesim_driver.c)

file(COPY traces DESTINATION .)
file(COPY run_script.sh DESTINATION .)
