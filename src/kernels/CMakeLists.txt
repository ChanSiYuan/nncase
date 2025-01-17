﻿cmake_minimum_required (VERSION 3.8)

set(SRCS convolution.cpp
         kernel_context.cpp
         nnil.cpp
         reduce_window.cpp
         tensor_compute.cpp)

if (BUILDING_RUNTIME)
    add_library(kernels OBJECT ${SRCS})
    target_include_directories(kernels PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(kernels PUBLIC gsl::gsl-lite mpark_variant::mpark_variant)
    set_property(TARGET kernels PROPERTY POSITION_INDEPENDENT_CODE ON)
    install(TARGETS kernels EXPORT nncaseruntimeTargets)
else()
    add_library(kernels OBJECT ${SRCS})
    target_include_directories(kernels PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(kernels PUBLIC gsl::gsl-lite mpark_variant::mpark_variant)
    target_link_libraries(kernels PRIVATE fmt::fmt)
    if(ENABLE_HALIDE)
        hkg_get_runtime_lib(hkg_runtime_lib os_name)
        hkg_get_suffix(obj_suffix lib_suffix)
        target_link_libraries(kernels PRIVATE hkg::${os_name}_src ${hkg_runtime_lib})
        if(os_name STREQUAL "linux")
            target_link_libraries(kernels PRIVATE -lpthread)
        endif()
        target_compile_definitions(kernels PRIVATE "-DNNCASE_HALIDE")        
    endif()
    
    target_compile_definitions(kernels PUBLIC -DNNCASE_DLL -DNNCASE_SIMULATOR)
    set_property(TARGET kernels PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

if(ENABLE_OPENMP)
    target_link_libraries(kernels PRIVATE OpenMP::OpenMP_CXX)
    target_compile_definitions(kernels PRIVATE "-DNNCASE_OPENMP")
endif()

add_subdirectory(cpu)
