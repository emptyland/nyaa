cmake_minimum_required(VERSION 2.8.12)
project(CppDemo CXX C)
enable_language(CXX)
enable_language(C)
enable_language(ASM)

# set(CMAKE_CXX_FLAGS "-std=c++17 -O2 -DNDEBUG -D_NDEBUG")
set(CMAKE_CXX_FLAGS "-std=c++17 -O0 -g -DDEBUG -D_DEBUG")

# Test requried libraries
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

find_library(GLUT_LIB glut)
if (NOT GLUT_LIB)
    message(FATAL_ERROR "glut library not found")
endif()

find_library(GL_LIB opengl)
if (NOT GL_LIB)
    message(FATAL_ERROR "opengl library not found")
endif()

include_directories(
    /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
add_executable(main main.cc)
target_link_libraries(main "-framework OpenGL")
target_link_libraries(main "-framework Glut")
target_link_libraries(main pthread dl ${GLEW_LIB} ${FREETYPE_LIB} ${LUA_LIB})

