cmake_minimum_required(VERSION 3.5)
project(JetAnalysor)

# Fail if cmake is called in the source directory
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "You don't want to configure in the source directory!")
endif()

set(CMAKE_CXX_STANDARD 14)

# Tell cmake where to find our modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Fastjet REQUIRED)
if(Fastjet_FOUND)
    include_directories(${Fastjet_INCLUDE_DIR})
    link_directories(${Fastjet_LIBRARIES})
endif()

find_package(HepMC REQUIRED)
if(HepMC_FOUND)
    include_directories(${HepMC_INCLUDE_DIRS})
    link_directories(${HepMC_LIBRARY_DIRS})
endif()

# find LBT library
find_package(LBT REQUIRED)
if(LBT_FOUND)
    include_directories(${LBT_INCLUDE_DIRS})
    link_directories(${LBT_LIBRARY_DIRS})
endif()

# 查找 OpenMP 支持
#find_package(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    message("OPENMP FOUND")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

# add 3rd-party libraries (before setting compiler flags etc)
#include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/Cuba-4.2")
# include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/Einhard")
# include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/yaml-cpp-0.6.2/include")
# add_subdirectory(3rdparty)

# 添加头文件目录
include_directories("include")

# Set -Wl,-rpath
set(CMAKE_SKIP_BUILD_RPATH TRUE)                 # 编译时加上RPATH
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)         # 编译时RPATH使用安装的RPATH
set(CMAKE_INSTALL_RPATH ${Fastjet_LIBRARIES})    # 安装RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)      # 安装的执行文件加上RPATH


#set(CMAKE_EXE_LINKER_FLAGS "-static")

# 添加编译优化选项
add_compile_options("-O3")

add_compile_options("-fpermissive")

add_executable(JetAnalysor main.cpp) # histo.cc)
target_link_libraries(JetAnalysor HepMC m fastjettools fastjet)

add_executable(runLBT runLBT.cpp)
target_link_libraries(runLBT HepMC LBT ) #HepMCfio)

add_executable(calc calc.cpp)
target_link_libraries(calc HepMC fastjettools fastjet)