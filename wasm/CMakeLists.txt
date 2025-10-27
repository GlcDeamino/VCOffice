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

if (FALSE)
qt_add_resources(VCOffice PREFIX "/JBM" FILES 
    res/ttf/JetBrainsMono-Bold.ttf
    res/ttf/JetBrainsMono-BoldItalic.ttf
    res/ttf/JetBrainsMono-ExtraBold.ttf
    res/ttf/JetBrainsMono-ExtraBoldItalic.ttf
    res/ttf/JetBrainsMono-ExtraLight.ttf
    res/ttf/JetBrainsMono-ExtraLightItalic.ttf
    res/ttf/JetBrainsMono-Italic.ttf
    res/ttf/JetBrainsMono-Light.ttf
    res/ttf/JetBrainsMono-LightItalic.ttf
    res/ttf/JetBrainsMono-Medium.ttf
    res/ttf/JetBrainsMono-MediumItalic.ttf
    res/ttf/JetBrainsMono-Regular.ttf
    res/ttf/JetBrainsMono-SemiBold.ttf
    res/ttf/JetBrainsMono-SemiBoldItalic.ttf
    res/ttf/JetBrainsMono-Thin.ttf
    res/ttf/JetBrainsMono-ThinItalic.ttf
    res/ttf/JetBrainsMonoNL-Bold.ttf
    res/ttf/JetBrainsMonoNL-BoldItalic.ttf
    res/ttf/JetBrainsMonoNL-ExtraBold.ttf
    res/ttf/JetBrainsMonoNL-ExtraBoldItalic.ttf
    res/ttf/JetBrainsMonoNL-ExtraLight.ttf
    res/ttf/JetBrainsMonoNL-ExtraLightItalic.ttf
    res/ttf/JetBrainsMonoNL-Italic.ttf
    res/ttf/JetBrainsMonoNL-Light.ttf
    res/ttf/JetBrainsMonoNL-LightItalic.ttf
    res/ttf/JetBrainsMonoNL-Medium.ttf
    res/ttf/JetBrainsMonoNL-MediumItalic.ttf
    res/ttf/JetBrainsMonoNL-Regular.ttf
    res/ttf/JetBrainsMonoNL-SemiBold.ttf
    res/ttf/JetBrainsMonoNL-SemiBoldItalic.ttf
    res/ttf/JetBrainsMonoNL-Thin.ttf
    res/ttf/JetBrainsMonoNL-ThinItalic.ttf
)
endif()

target_sources(VCOffice PRIVATE
    src/common/RTButton.cpp
    src/ppt_player/PPT_Player.cpp
    src/decoder/docx_decoder.cpp
    src/decoder/decompressor.cpp
    src/word/WordEditor.cpp
    src/word/fntabs/StartTab.cpp
    src/word/fntabs/start/ClipboardSec.cpp
    src/word/fntabs/start/FontSec.cpp
)

target_link_libraries(VCOffice PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
)
