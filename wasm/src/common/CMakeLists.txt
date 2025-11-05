add_library(vc_common 
    RTButton.cpp
    start/ClipboardSec.cpp
    start/FontSec.cpp
    OfficeItem.cpp)
target_include_directories(vc_common PUBLIC .)

# 命名空间导出
add_library(VCOffice::Common ALIAS vc_common)

target_link_libraries(vc_common PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg)