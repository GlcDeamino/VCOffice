add_library(vc_ppt_player PPT_Player.cpp)
target_include_directories(vc_ppt_player PUBLIC .)
add_library(VCOffice::PPTPlayer ALIAS vc_ppt_player)

target_link_libraries(vc_ppt_player PUBLIC Qt6::Core Qt6::Widgets)