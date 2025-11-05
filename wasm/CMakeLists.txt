cmake_minimum_required(VERSION 3.16)

project(VCOffice VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Xml Svg)
qt_standard_project_setup()

# 添加源码目录
add_subdirectory(src)

qt_add_resources(vc_common "svg"
    PREFIX "/"
    FILES 
        res/svg/content-cut.svg
        res/svg/content-copy.svg
        res/svg/content-paste.svg
        res/svg/CursorArrow.svg
)