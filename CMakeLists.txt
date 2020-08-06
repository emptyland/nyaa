cmake_minimum_required(VERSION 2.8.12)
project(Nyaa CXX C)
enable_language(CXX)
enable_language(C)
enable_language(ASM)

# set(CMAKE_CXX_FLAGS "-std=c++17 -O2 -DNDEBUG -D_NDEBUG")
set(CMAKE_CXX_FLAGS "-std=c++17 -O0 -g -DDEBUG -D_DEBUG -Wno-deprecated-declarations")

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
)

include_directories(
    /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks
    /usr/local/include
    src
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
add_executable(nyaa ${GAME_SRC})
target_link_libraries(nyaa "-framework OpenGL")
target_link_libraries(nyaa "-framework Glut")
target_link_libraries(nyaa pthread dl ${GLFW_LIB} ${GLEW_LIB} ${FREETYPE_LIB} ${LUA_LIB} ${GLOG_LIB})

