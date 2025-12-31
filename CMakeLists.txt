# define the minimum cmake version
cmake_minimum_required(VERSION 3.50)

# define the project name
project(whitespace VERSION 1.0 LANGUAGES CXX )

# define the c standard version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)


include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# raylib als SYSTEM include markieren
get_target_property(RAYLIB_INCLUDE_DIRS raylib INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(raylib
    SYSTEM INTERFACE ${RAYLIB_INCLUDE_DIRS}
)

# add some compile options
add_compile_options(-Wall -Wextra)

add_subdirectory(vendor/yaml-cpp)

add_library(${PROJECT_NAME}_lib STATIC src/Application.cpp
                                       src/CanvasLayer.cpp
                                       src/Layer.cpp
                                       src/UILayer.cpp
                                       src/Inspector.cpp
                                       src/AssetExplorer.cpp
                                       src/AssetManager.cpp
                                       src/DataPersistanceLayer.cpp
)

# add an executabel and assign files to it
add_executable(${PROJECT_NAME}_exec src/main.cpp)

# include raylib.h
# SYSTEM to supress warning (since it' a C API)
target_include_directories(
  ${PROJECT_NAME}_exec SYSTEM PRIVATE vendor
                                    styles/candy
)

target_include_directories(
  ${PROJECT_NAME}_lib SYSTEM PRIVATE vendor
                                    styles/candy
)

target_include_directories(${PROJECT_NAME}_lib PRIVATE includes)
target_include_directories(${PROJECT_NAME}_exec PRIVATE includes)

target_link_libraries(${PROJECT_NAME}_exec PRIVATE raylib)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE raylib yaml-cpp)
target_link_libraries(${PROJECT_NAME}_exec PRIVATE ${PROJECT_NAME}_lib yaml-cpp)
