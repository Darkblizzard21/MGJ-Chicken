project(engine)
add_library(engine STATIC)

target_sources(engine
    PRIVATE
    include/App.h      
    include/DeferredComposit.h   
    include/NumberManager.h   
    include/Quad.h 
    include/Shader.h     
    include/Texture.h   
    include/Timer.h 
    include/UberShader.h


    App.cpp         
    DeferredComposit.cpp
    NumberManager.cpp
    Quad.cpp
    Shader.cpp
    Texture.cpp
    Timer.cpp
    UberShader.cpp
)
target_link_libraries(engine
    PRIVATE
    glfw
    glad
    glm::glm
    stbi
    box2d
    soloud
)

target_include_directories(engine PUBLIC include)
