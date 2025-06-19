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

option(ENABLE_RS_TRACE "Enable RS tracing functionality" OFF)
if(ENABLE_RS_TRACE)
    add_definitions(-DENABLE_RS_TRACE)
endif()
# source file
file(GLOB SOURCES_FILES
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

# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RS/
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RS
#     FILES_MATCHING PATTERN "*.h"
# )
# 安装主头文件
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RS.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RS_internal.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RS
)
# 安装pattern目录下的头文件
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pattern/pattern.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RS/pattern
)

# 创建 rcwasolver.pc.in 模板文件
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/rcwasolver.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/rcwasolver.pc"
    @ONLY
)

# 安装 pc 文件
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rcwasolver.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
#     FILES_MATCHING PATTERN "*.pc"
# )



if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# 通用打包配置
set(CPACK_PACKAGE_NAME "rcwasolver")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Solver for RCWA")
set(CPACK_PACKAGE_VENDOR "YuWei Optica Inc")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_HOST_SYSTEM_PROCESSOR}")

# 根据系统类型设置不同的打包生成器和选项
if(EXISTS "/etc/redhat-release")  # CentOS/RHEL
    set(CPACK_GENERATOR "RPM")
    set(CPACK_PACKAGE_RELEASE "0")
    set(CPACK_RPM_PACKAGE_GROUP "rcwasolver")
    set(CPACK_RPM_PACKAGE_LICENSE "")
    set(CPACK_RPM_PACKAGE_REQUIRES "")
    set(CPACK_RPM_PACKAGE_AUTOREQ "no")
elseif(EXISTS "/etc/debian_version")  # Ubuntu/Debian
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "YuWei Optica Inc")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
else()  # 其他系统使用TGZ作为后备
    set(CPACK_GENERATOR "TGZ")
endif()

# 设置安装前缀
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")

include(CPack)


# # installer
# include(GNUInstallDirs)
# install(TARGETS rcwasolver
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/RS/
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RS
#     FILES_MATCHING PATTERN "*.h"
# )

# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
#     FILES_MATCHING PATTERN "*.pc"
# )

# if(NOT TARGET uninstall)
#   configure_file(
#     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
#     "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
#     IMMEDIATE @ONLY)

#   add_custom_target(uninstall
#     COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake)
# endif()

# set(CPACK_GENERATOR "RPM")
# set(CPACK_PACKAGE_NAME "rcwasolver")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Solver for RCWA")
# set(CPACK_PACKAGE_VENDOR "YuWei Optica Inc")
# set(CPACK_PACKAGE_VERSION "1.0.0")
# set(CPACK_PACKAGE_VERSION_MAJOR "1")
# set(CPACK_PACKAGE_VERSION_MINOR "0")
# set(CPACK_PACKAGE_VERSION_PATCH "0")
# set(CPACK_PACKAGE_RELEASE "0")
# set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_HOST_SYSTEM_PROCESSOR})
# set(CPACK_RPM_PACKAGE_GROUP "rcwasolver")
# set(CPACK_RPM_PACKAGE_LICENSE "")

# include(CPack)

