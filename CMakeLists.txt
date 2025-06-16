cmake_minimum_required(VERSION 3.23)
project(rcwa_solver VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

# import 3rdparty
#     mkl
set(MKL_THREADING gnu_thread)
set(MKL_MPI openmpi)
set(MKL_LINK static)
if(EXISTS "/opt/intel/oneapi/mkl/latest/lib/cmake/mkl/")
    set(MKL_DIR "/opt/intel/oneapi/mkl/latest/lib/cmake/mkl/")
    set(MKL_ARCH intel64)
    set(MKL_INTERFACE lp64)
    find_package(MKL CONFIG REQUIRED)
else()
    find_package(MKL CONFIG REQUIRED)
endif()
#     eigen3
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})
#     fftw
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
include_directories(PkgConfig::FFTW )
link_libraries     (PkgConfig::FFTW)
pkg_search_module(FFTWF REQUIRED fftw3f IMPORTED_TARGET)
include_directories(PkgConfig::FFTWF )
link_libraries     (PkgConfig::FFTWF)

# source file
file(GLOB SOURCES_FILES
    src/main.cpp
    src/Eigensystems.cpp
    src/RS.cpp
    src/gsel.c
    src/sort.c
    src/numalloc.c
    src/IRA.cpp
    src/rcwa.cpp

    src/fmm/fft_iface.cpp
    src/fmm/fmm_common.cpp
    src/fmm/fmm_kottke.cpp
    src/fmm/fmm_FFT.cpp
    src/fmm/fmm_PolBasisJones.cpp
    src/fmm/fmm_closed.cpp
    src/fmm/fmm_experimental.cpp
    src/fmm/fmm_PolBasisNV.cpp
    src/fmm/fmm_PolBasisVL.cpp
    
    src/RNP/Eigensystems.cpp
    
    src/pattern/pattern.c
    src/pattern/intersection.c
    src/pattern/predicates.c
)
add_library(rcwasolver SHARED ${SOURCES_FILES})
target_include_directories(rcwasolver
    PUBLIC include
    PUBLIC include/fmm
    PUBLIC include/RNP
    PUBLIC include/pattern
)
target_link_libraries(rcwasolver PUBLIC MKL::MKL)

# tests
add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/tests/example.cpp)
target_link_libraries(example PUBLIC rcwasolver)

# installer
include(GNUInstallDirs)
install(TARGETS rcwasolver
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RS/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RS
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    FILES_MATCHING PATTERN "*.pc"
)

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake)
endif()

set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "rcwasolver")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Solver for RCWA")
set(CPACK_PACKAGE_VENDOR "YuWei Optica Inc")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_RELEASE "0")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_HOST_SYSTEM_PROCESSOR})
set(CPACK_RPM_PACKAGE_GROUP "rcwasolver")
set(CPACK_RPM_PACKAGE_LICENSE "")

include(CPack)

