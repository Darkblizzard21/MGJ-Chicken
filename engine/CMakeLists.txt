project(engine)
add_library(engine STATIC)

target_sources(engine
    PRIVATE
    include/App.h   
    include/Shader.h 
    include/Quad.h
       
    App.cpp
    Shader.cpp
    Quad.cpp
)
target_link_libraries(engine
    PRIVATE
    glfw
    glad
    glm::glm
)

target_include_directories(engine PUBLIC include)
