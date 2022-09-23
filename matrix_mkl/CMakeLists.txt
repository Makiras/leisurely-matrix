cmake_minimum_required(VERSION 3.0.2)
project(matrix_mkl)

include_directories(/opt/intel/oneapi/mkl/2022.1.0/include include)

link_directories(/opt/intel/oneapi/mkl/2022.1.0/lib/intel64)

# check NI define is number
if (NOT DEFINED NI)
    set(NI 1024)
endif()
message( "NI is "${NI} )
add_definitions(-DNI=${NI})
add_executable(matrix_mkl matrix_mkl.c)

target_link_libraries(matrix_mkl libmkl_rt.so pthread libm.so libdl.so)
