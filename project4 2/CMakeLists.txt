cmake_minimum_required(VERSION 3.2)
project(vm-sim)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99 -pedantic -pipe -Werror ")

set(student_source_files
        student-src/address_split.c
        student-src/compute_stats.c
        student-src/page_fault.c
        student-src/page_lookup.c
        student-src/tlb_lookup.c
        )

set(sim_source_files
        simulator-src/global.c
        simulator-src/main.c
        simulator-src/pagetable.c
        simulator-src/process.c
        simulator-src/reverselookup.c
        simulator-src/tlb.c
        simulator-src/util.c
        )

include_directories(${PROJECT_SOURCE_DIR}/student-src)
include_directories(${PROJECT_SOURCE_DIR}/simulator-src)

add_executable(vm-sim ${sim_source_files} ${student_source_files})

file(COPY run_script.sh DESTINATION .)
file(COPY traces DESTINATION .)
