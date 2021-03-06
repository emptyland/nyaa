# cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake_minimum_required(VERSION 2.8.12)
project(Nyaa CXX C)
enable_language(CXX)
enable_language(C)
enable_language(ASM)

message(STATUS "Operation system is ${CMAKE_SYSTEM}")

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG -D_NDEBUG")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined-trap -O0 -g -DDEBUG -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG -D_DEBUG")

# Test requried libraries
find_library(GLOG_LIB glog)
if(NOT GLOG_LIB)
  message(FATAL_ERROR "glog library not found")
endif()

find_library(LUA_LIB lua)
if(NOT LUA_LIB)
  message(FATAL_ERROR "lua library not found")
endif()

find_library(FREETYPE_LIB freetype)
if (NOT FREETYPE_LIB)
    message(FATAL_ERROR "freetype library not found")
endif()

find_library(GLEW_LIB glew)
if (NOT GLEW_LIB)
    message(FATAL_ERROR "glew library not found")
endif()

find_library(GLFW_LIB glfw)
if (NOT GLFW_LIB)
    message(FATAL_ERROR "glfw library not found")
endif()

find_library(GL_LIB opengl)
if (NOT GL_LIB)
    message(FATAL_ERROR "opengl library not found")
endif()

# Define source files
set(GAME_SRC
    src/main.cc
    src/game/game.cc
    src/game/properties.cc
    src/game/entity-virtual-db.cc
    src/game/entity-grid-set.cc
    src/game/matrix.cc
    src/game/world-def.cc
    src/game/world.cc
    src/scene/scene.cc
    src/scene/boot-scene.cc
    src/scene/test-scene.cc
    src/scene/avatar-view-scene.cc
    src/scene/tile-view-scene.cc
    src/scene/cube-view-scene.cc
    src/scene/world-creation-scene.cc
    src/ui/component.cc
    src/ui/ui-service.cc
    src/ui/input-box.cc
    src/ui/list-box.cc
    src/ui/button-group.cc
    src/ui/item-grid-view.cc
    src/ui/flat-menu.cc
    src/ui/label-check-box.cc
    src/ui/check-box-group.cc
    src/ui/avatar-selector.cc
    src/ui/property-box.cc
    src/ui/property-box-group.cc
    src/ui/label-component.cc
    src/ui/message-box.cc
    src/ui/label-text-box.cc
    src/entity/player-entity.cc
    src/entity/actor-entity.cc
    src/component/cube-component.cc
    src/component/zone-component.cc
    src/component/movement-component.cc
    src/system/random-zone-system.cc
    src/system/zone-render-system.cc
    src/system/zone-loading-system.cc
    src/system/avatar-render-system.cc
    src/system/actor-movement-system.cc
    src/system/geometry-transform-system.cc
    src/system/sprite-render-system.cc
    src/system/actor-ai-system.cc
    src/system/actor-billboard-render-system.cc
    src/system/impact-checking-system.cc
    src/system/world-generating-system.cc
    src/system/system.cc
    src/resource/font-library.cc
    src/resource/text-library.cc
    src/resource/texture-library.cc
    src/resource/sprite-library.cc
    src/resource/cube-library.cc
    src/resource/avatar-library.cc
    src/resource/shader-library.cc
    src/resource/actor-library.cc
    src/resource/skill-library.cc
    src/resource/definition.cc
    src/resource/ai-def.cc
    src/resource/text-formatter.cc
    src/base/arena-utils.cc
    src/base/arenas.cc
    src/base/hash.cc
    src/base/varint-encoding.cc
    src/base/arena.cc
    src/base/at-exit.cc
    src/base/slice.cc
    src/base/zone.cc
    src/base/crc32.c
    src/base/sha256.c
    third_party/lodepng/lodepng.cpp
    third_party/lodepng/lodepng_util.cpp
)

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include_directories(
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks
        /usr/local/include/freetype2
        /usr/local/include
    )
endif()

include_directories(
    third_party/lodepng
    src
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
add_executable(nyaa ${GAME_SRC})
target_link_libraries(nyaa "-framework OpenGL")
target_link_libraries(nyaa "-framework Glut")
target_link_libraries(nyaa pthread dl ${GLFW_LIB} ${GLEW_LIB} ${FREETYPE_LIB} ${LUA_LIB} ${GLOG_LIB})

