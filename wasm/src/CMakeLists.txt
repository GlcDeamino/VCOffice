# 定义主可执行文件
qt_add_executable(VCOffice main.cpp)

# 链接所有子模块
target_link_libraries(VCOffice PRIVATE
    VCOffice::Common
    VCOffice::Decoder
    VCOffice::PPTPlayer
    VCOffice::Word
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(VCOffice PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加子目录
add_subdirectory(common)
add_subdirectory(decoder)
add_subdirectory(ppt_player)
add_subdirectory(word)