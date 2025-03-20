cmake_minimum_required(VERSION 3.30)
project(Realsense_cloud)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Realsense2 REQUIRED)

include(FetchContent)
set(THREEPP_BUILD_TESTS OFF)
set(THREEPP_BUILD_EXAMPLES OFF)
FetchContent_Declare(
        threepp
        GIT_REPOSITORY https://github.com/markaren/threepp.git
        GIT_TAG edd2597344f21cc491f65354849dee1d66c2e427
)
FetchContent_MakeAvailable(threepp)

add_executable(Realsense_cloud main.cpp)
target_link_libraries(Realsense_cloud PRIVATE threepp realsense2::realsense2)
