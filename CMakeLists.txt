cmake_minimum_required(VERSION 3.27)
project(PTCCP)

set(CMAKE_CXX_STANDARD 14)


add_executable(PTCCP main.cpp
        Cache_Functions.cpp
        PrivateCache.h
        SysConf.h
        TM_functions.cpp
        TM.h
        tb.cpp
        tb.h
        Bus.h
        Bus_functions.cpp
        MEM_SYSTEM.h)


target_include_directories(PTCCP PRIVATE "/opt/systemc/include")
target_link_directories(PTCCP PRIVATE "/opt/systemc/lib")
target_link_libraries(PTCCP PRIVATE systemc)
