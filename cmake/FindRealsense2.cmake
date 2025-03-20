# FindRealsense2.cmake
# Locate the Intel RealSense library

find_path(REALSENSE2_INCLUDE_DIR NAMES librealsense2/rs.hpp
        PATHS
        "$ENV{REALSENSE2_DIR}/include"
)

find_library(REALSENSE2_LIBRARY NAMES realsense2
        PATHS
        "$ENV{REALSENSE2_DIR}/lib/x64"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Realsense2 DEFAULT_MSG
        REALSENSE2_INCLUDE_DIR
        REALSENSE2_LIBRARY)

if(REALSENSE2_FOUND)
    set(REALSENSE2_LIBRARIES ${REALSENSE2_LIBRARY})
    set(REALSENSE2_INCLUDE_DIRS ${REALSENSE2_INCLUDE_DIR})

    add_library(realsense2::realsense2 INTERFACE IMPORTED)
    set_target_properties(realsense2::realsense2 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${REALSENSE2_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES "${REALSENSE2_LIBRARIES}"
    )
endif()
