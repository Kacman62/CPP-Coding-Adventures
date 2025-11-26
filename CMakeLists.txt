cmake_minimum_required(VERSION 3.14)
project(MyFTXUIProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add FTXUI as a subdirectory
add_subdirectory(third_party/FTXUI)

# Your executable
add_executable(my_app main.cpp)

# Link FTXUI libraries
target_link_libraries(my_app PRIVATE ftxui::dom ftxui::component)
