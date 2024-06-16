# CMakeList.txt : CMake project for computergraphics, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable(game)

target_sources(game
    PRIVATE
    main.cpp
    Minecart.h
    Minecart.cpp)
target_link_libraries(game
    PRIVATE
    engine
    glfw
    glad
    glm::glm
)