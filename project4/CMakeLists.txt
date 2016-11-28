cmake_minimum_required(VERSION 3.2)
project(vm-sim)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99 -pedantic ")
set(SOURCE_FILES
    simulator-src/global.c
    simulator-src/global.h
    simulator-src/main.c
    simulator-src/pagetable.c
    simulator-src/pagetable.h
    simulator-src/process.c
    simulator-src/process.h
    simulator-src/reverselookup.c
    simulator-src/reverselookup.h
    simulator-src/stats.h
    simulator-src/tlb.c
    simulator-src/tlb.h
    simulator-src/util.c
    simulator-src/util.h
    student-src/address_split.c
    student-src/compute_stats.c
    student-src/page_fault.c
    student-src/page_lookup.c
    student-src/tlb_lookup.c
    traces/astar_vm.trace
    traces/bzip2_vm.trace
    traces/mcf_vm.trace
    traces/perlbench_vm.trace
    Makefile
    README.md
    run_script.sh
    solution.log)


add_executable(vm-sim ${SOURCE_FILES})

file(COPY traces DESTINATION .)
file(COPY run_script.sh DESTINATION .)


#cmake_minimum_required(VERSION 3.6)
#project(project4)
#
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#
#set(SOURCE_FILES
#    simulator-src/global.c
#    simulator-src/global.h
#    simulator-src/main.c
#    simulator-src/pagetable.c
#    simulator-src/pagetable.h
#    simulator-src/process.c
#    simulator-src/process.h
#    simulator-src/reverselookup.c
#    simulator-src/reverselookup.h
#    simulator-src/stats.h
#    simulator-src/tlb.c
#    simulator-src/tlb.h
#    simulator-src/util.c
#    simulator-src/util.h
#    student-src/address_split.c
#    student-src/compute_stats.c
#    student-src/page_fault.c
#    student-src/page_lookup.c
#    student-src/tlb_lookup.c
#    traces/astar_vm.trace
#    traces/bzip2_vm.trace
#    traces/mcf_vm.trace
#    traces/perlbench_vm.trace
#    Makefile
#    README.md
#    run_script.sh
#    solution.log)
#
#add_executable(project4 ${SOURCE_FILES})