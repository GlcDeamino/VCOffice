add_library(vc_word 
    WordEditor.cpp
    WordScene.cpp
    fntabs/StartTab.cpp
    items/RT.cpp
    layout/CR.cpp
    layout/Page.cpp
    layout/Para.cpp
)
target_include_directories(vc_word PUBLIC .)
add_library(VCOffice::Word ALIAS vc_word)

target_link_libraries(vc_word PUBLIC 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Svg
    VCOffice::Common
    VCOffice::Decoder
)