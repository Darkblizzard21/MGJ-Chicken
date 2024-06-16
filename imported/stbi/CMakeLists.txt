project(stbi)
add_library(stbi STATIC)
TARGET_SOURCES(stbi 
    PRIVATE
    include/stb_image.h
    include/stb_image_write.h

    stb_build.cpp
    )

target_include_directories(stbi PUBLIC include)