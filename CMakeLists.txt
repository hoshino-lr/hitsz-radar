cmake_minimum_required(VERSION 3.0.0)
project(radar VERSION 0.2)

include(CTest)
enable_testing()

message(STATUS "${PROJECT_NAME} started building on ${CMAKE_SYSTEM_NAME}!")
# Options normal
    set(use_camera_usb2 false)     # 是否使用电脑自带摄像头做调试
    set(camera_id 0)
    # 是否使用虚拟Serial
    #set(use_fake_ttl false)
    set(use_fake_ttl true)
    # 是否编译装甲板
#    set(compile_armor_detect false)
    set(compile_armor_detect true)
#    set(compile_cuda true)
    # 是否PNP
    set(compile_pnp true)
#    set(compile_pnp false)
configure_file(
        "${PROJECT_SOURCE_DIR}/include/compile_config.h.in"
        "${PROJECT_SOURCE_DIR}/include/compile_config.h"
)

# Packages
# OpenCV
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}, lib: ${OpenCV_LIBS}")
include_directories(${OpenCV_INCLUDE_DIRS})
# glog
find_package(glog REQUIRED)
# Galaxy_U2
# set(GALAXY_PATH "/home/hoshino/Galaxy_Linux_x86_U2_CN_1.0.2011.9171/Galaxy_U2")
set(GALAXY_PATH "~/Galaxy_U2")

# Include
if (compile_armor_detect)
    set(include_dirs
            "${GALAXY_PATH}/sdk/include"
            "${GALAXY_PATH}/sdk/genicam/library/CPP/include/library/CPP/include"
            "${PROJECT_SOURCE_DIR}/include"
            "${PROJECT_SOURCE_DIR}/src/armor/include" "${PROJECT_SOURCE_DIR}/src/armor/src"
            "${PROJECT_SOURCE_DIR}/src/mapping/include" "${PROJECT_SOURCE_DIR}/src/mapping"
            "${PROJECT_SOURCE_DIR}/src/rmcamera/include" "${PROJECT_SOURCE_DIR}/src/rmcamera"
            "${PROJECT_SOURCE_DIR}/src/rmconfig/include" "${PROJECT_SOURCE_DIR}/src/rmconfig"
            "${PROJECT_SOURCE_DIR}/src/minimap/include" "${PROJECT_SOURCE_DIR}/src/minimap"
            "${PROJECT_SOURCE_DIR}/src/com/include" "${PROJECT_SOURCE_DIR}/src/com"
            "${PROJECT_SOURCE_DIR}/src/others/include")
else (compile_armor_detect)
    set(include_dirs
            "${PROJECT_SOURCE_DIR}/include"
            "${GALAXY_PATH}/sdk/include"
            "${GALAXY_PATH}/sdk/genicam/library/CPP/include/library/CPP/include"
            "${PROJECT_SOURCE_DIR}/src/rmcamera/include" "${PROJECT_SOURCE_DIR}/src/rmcamera"
            "${PROJECT_SOURCE_DIR}/src/rmconfig/include" "${PROJECT_SOURCE_DIR}/src/rmconfig"
            "${PROJECT_SOURCE_DIR}/src/others/include")
endif (compile_armor_detect)
message(STATUS "Include dirs: ${include_dirs}")
include_directories(${include_dirs})

# Src

file(GLOB_RECURSE source_files
        "${PROJECT_SOURCE_DIR}/src/**/include/*.h" "${PROJECT_SOURCE_DIR}/src/**/src/*.?pp"
        "${PROJECT_SOURCE_DIR}/src/com/include/*.h" "${PROJECT_SOURCE_DIR}/src/com/include/serial/*.h"
        "${PROJECT_SOURCE_DIR}/src/com/include/serial/impl/*.h"
        "${PROJECT_SOURCE_DIR}/src/com/src/*.cc" "${PROJECT_SOURCE_DIR}/src/com/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/com/src/impl/*.cc" "${PROJECT_SOURCE_DIR}/src/com/src/impl/list_ports/*.cc"
        "${PROJECT_SOURCE_DIR}/src/rmcamera/include/*" "${PROJECT_SOURCE_DIR}/src/rmcamera/src/*"
        "${PROJECT_SOURCE_DIR}/src/rmconfig/include/*" "${PROJECT_SOURCE_DIR}/src/rmconfig/src/*"
        "${PROJECT_SOURCE_DIR}/src/minimap/include/*" "${PROJECT_SOURCE_DIR}/src/minimap/src/*"
        "${PROJECT_SOURCE_DIR}/src/others/include/**/*.h" "${PROJECT_SOURCE_DIR}/src/others/src/*.?pp"
        "${PROJECT_SOURCE_DIR}/src/others/src/camera/*.?pp"
        "${PROJECT_SOURCE_DIR}/src/others/src/lib_json/*.?pp"
        "${PROJECT_SOURCE_DIR}/src/armor/src/armor_finder/armor_finder.cpp"
        "${PROJECT_SOURCE_DIR}/src/mapping/src/3Dpnp.cpp" "${PROJECT_SOURCE_DIR}/src/mapping/include/3Dpnp.h"
        "${PROJECT_SOURCE_DIR}/src/net/include/*.h" "${PROJECT_SOURCE_DIR}/src/net/include/serial/*.h"
        )
message(STATUS "Source files: ${source_files}")

if (use_camera_usb2)
    list(REMOVE_ITEM source_files "${PROJECT_SOURCE_DIR}/src/others/include/camera/cam_wrapper.h")
    list(REMOVE_ITEM source_files "${PROJECT_SOURCE_DIR}/src/others/src/camera/cam_wrapper.cpp")
endif ()

# Resources

file(GLOB_RECURSE resources "${PROJECT_SOURCE_DIR}/resource/*")
message(STATUS "Copying resources file(s): ${resources}")
file(COPY ${resources} DESTINATION ${CMAKE_BINARY_DIR}/)

# Linking
message(STATUS "u2_camera_root $ENV{GENICAM_ROOT_V2_3}")

link_directories(${GALAXY_PATH}/sdk/genicam/bin/Linux64_x64)
link_directories(${GALAXY_PATH}/sdk/genicam/bin/Linux64_x64/GenApi/Generic)
link_libraries(-ldhgentl -lpthread)
link_libraries(-ldx_image_proc -lGCBase_gcc40_v2_3 -lGenApi_gcc40_v2_3 -llog4cpp_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3)
add_executable(camera-test ${PROJECT_SOURCE_DIR}/programs/radar_camera-test.cpp ${source_files})
target_link_libraries(camera-test ${OpenCV_LIBS} -lglog -lgxiapi -lpthread)
#add_executable(radar ${PROJECT_SOURCE_DIR}/programs/main.cpp ${source_files})
if (compile_pnp)
    add_executable(radar_pnp ${PROJECT_SOURCE_DIR}/programs/pnp_test.cpp ${source_files} )
    target_link_libraries(radar_pnp ${OpenCV_LIBS} -lglog -lgxiapi -lpthread)
endif (compile_pnp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
