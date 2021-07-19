if(NOT PKG_CONFIG_FOUND)
    INCLUDE(FindPkgConfig)
endif()
PKG_CHECK_MODULES(PC_SCHED sched)

FIND_PATH(
    SCHED_INCLUDE_DIRS
    NAMES sched/api.h
    HINTS $ENV{SCHED_DIR}/include
        ${PC_SCHED_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SCHED_LIBRARIES
    NAMES gnuradio-sched
    HINTS $ENV{SCHED_DIR}/lib
        ${PC_SCHED_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/schedTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SCHED DEFAULT_MSG SCHED_LIBRARIES SCHED_INCLUDE_DIRS)
MARK_AS_ADVANCED(SCHED_LIBRARIES SCHED_INCLUDE_DIRS)
