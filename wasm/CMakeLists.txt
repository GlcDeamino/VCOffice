cmake_minimum_required(VERSION 3.16)

project(VCOffice VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
)

qt_standard_project_setup()

qt_add_executable(VCOffice src/main.cpp)

target_sources(VCOffice PRIVATE
    src/ppt_player/PPT_Player.cpp
    src/decoder/docx_decoder.cpp
    src/decoder/decompressor.cpp
    src/word/WordEditor.cpp
)

target_link_libraries(VCOffice PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
)
