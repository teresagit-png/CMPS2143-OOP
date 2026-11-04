cmake_minimum_required(VERSION 3.20)
project(VidPoker LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(ftxui
  URL https://github.com/ArthurSonzogni/FTXUI/archive/refs/tags/v6.1.9.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ftxui)

set(STARTER ${CMAKE_CURRENT_SOURCE_DIR}/../starter)  # pulls in ftxui::dom and ftxui::screen

file(GLOB VidPoker_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM VidPoker_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Tests.cpp"
)

add_executable(VideoPoker ${VidPoker_SOURCES} 
    ${STARTER}/src/Card.cpp 
    ${STARTER}/src/Deck.cpp)

if(NOT MSVC)
  target_compile_options(VideoPoker PRIVATE -Wall -Wextra)
endif()
target_include_directories(VideoPoker PRIVATE include ${STARTER}/include)

target_link_libraries(VideoPoker PRIVATE ftxui::component)

if(MINGW)
  target_link_options(VideoPoker PRIVATE -static)
endif()