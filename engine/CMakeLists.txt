project(engine)
add_library(engine STATIC)

target_sources(engine
    PRIVATE
    include/main.h
    main.cpp
)
target_link_libraries(engine
    PRIVATE
    glfw
    glad
    glm::glm
)

target_include_directories(engine PUBLIC include)
