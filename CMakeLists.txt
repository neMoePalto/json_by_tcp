cmake_minimum_required(VERSION 3.5)

project(json_by_tcp LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)


find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Network
  Widgets
)

add_subdirectory(src/client_data_sender)

add_subdirectory(src/server_data_receiver)
