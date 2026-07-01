cmake_minimum_required(VERSION 3.16)
project(MyGame CXX)  # tells CMake this is a C++ project

set(CMAKE_CXX_STANDARD 17)          # use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # enforce it

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
)
FetchContent_MakeAvailable(raylib)

add_executable(MyGame 
src/main.cpp
src/Frontend/Implementation/Homedesign.cpp
src/Frontend/Implementation/shuffling.cpp
src/Frontend/Implementation/HandleMusic.cpp
src/Frontend/Implementation/score.cpp
src/Frontend/Implementation/RendererInterface.cpp
src/Frontend/Implementation/bid_screen.cpp
src/Frontend/Implementation/Dealing_Cards.cpp
src/Backend/Implementation/entities.cpp
src/Backend/Implementation/GameManager.cpp
src/Backend/Implementation/TimeManager.cpp
src/Backend/Implementation/RoundManager.cpp
)
target_link_libraries(MyGame raylib)

if(WIN32)
    target_link_options(MyGame PRIVATE -static)
endif()
