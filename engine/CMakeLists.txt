project(engine)
add_library(engine STATIC)

target_sources(engine
    PRIVATE
    include/App.h   
    include/Shader.h     
    include/Texture.h 
    include/Quad.h
     
    App.cpp
    Shader.cpp
    Texture.cpp
    Quad.cpp
)
target_link_libraries(engine
    PRIVATE
    glfw
    glad
    glm::glm
    stbi
)

target_include_directories(engine PUBLIC include)
