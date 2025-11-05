add_library(vc_decoder 
    docx_decoder.cpp 
    decompressor.cpp
    word_item/Paragraph.cpp
    word_item/RichText.cpp
    word_item/Section.cpp
)
target_include_directories(vc_decoder PUBLIC .)
add_library(VCOffice::Decoder ALIAS vc_decoder)

target_link_libraries(vc_decoder PUBLIC
    Qt6::Core
    Qt6::Xml
    VCOffice::Common
    VCOffice::Word
)