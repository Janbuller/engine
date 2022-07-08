cmake_minimum_required(VERSION 3.12)
project(ecs-test VERSION 0.1.0)

include(FetchContent)

# Include glfw #
#==============#
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# Include glad #
#==============#
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
)

FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
    set(GLAD_API "gl=4.6" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest")
    set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
endif()

# Include glm #
#=============#
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
  add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

# Include spdlog #
#================#
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG v1.x
)

FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)

  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.1.3
)

# Include assimp #
#================#
FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
  FetchContent_Populate(assimp)

  set( ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "If the test suite for Assimp is built in addition to the library." )
  set( ASSIMP_INSTALL OFF CACHE INTERNAL "Disable this if you want to use assimp as a submodule." )
  set( ASSIMP_NO_EXPORT ON CACHE INTERNAL "Disable Assimp's export functionality." )

  set( ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF )
  set( ASSIMP_BUILD_FBX_IMPORTER ON )
  set( ASSIMP_BUILD_OBJ_IMPORTER ON )
  set( ASSIMP_BUILD_GLTF_IMPORTER ON )
  set( ASSIMP_BUILD_BLEND_IMPORTER ON )

  add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
endif()

# Include Lua #
#=============#
find_package(Lua REQUIRED)

# Include LuaBridge #
#===================#
FetchContent_Declare(
        luabridge
        GIT_REPOSITORY https://github.com/vinniefalco/LuaBridge.git
)

FetchContent_GetProperties(luabridge)
if(NOT luabridge_POPULATED)
    FetchContent_Populate(luabridge)

    set(LUABRIDGE_CXX17 ON CACHE INTERNAL "Use C++17 standard if supported by compiler")

    add_subdirectory(${luabridge_SOURCE_DIR} ${luabridge_BINARY_DIR})
endif()

# Include re2 #
#=============#
FetchContent_Declare(
        re2
        GIT_REPOSITORY https://github.com/google/re2.git
	GIT_TAG main
)

FetchContent_GetProperties(re2)

if(NOT re2_POPULATED)
    FetchContent_Populate(re2)

    set(RE2_BUILD_TESTING OFF CACHE INTERNAL "enable testing for RE2")

    add_subdirectory(${re2_SOURCE_DIR} ${re2_BINARY_DIR})
endif()

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)

if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

file(GLOB_RECURSE ecs-test_src
  "src/*.cpp"
)

add_executable(ecs-test
  ${ecs-test_src}
)

set_target_properties(ecs-test PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)

target_include_directories(ecs-test PRIVATE include ${LUA_INCLUDE_DIR})

target_link_libraries(ecs-test glfw glm glad spdlog assimp ${LUA_LIBRARIES} LuaBridge re2::re2)
