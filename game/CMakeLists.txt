﻿# CMakeList.txt : CMake project for computergraphics, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable(game)

target_sources(game
    PRIVATE
    main.cpp
    Minecart.h
    Minecart.cpp
    Chickenwings.h
    Chickenwings.cpp
    Spline.h
    Spline.cpp
    SplineRenderer.h
    SplineRenderer.cpp
    PickUp.h
    PickUp.cpp
    ContactListener.h
    ContactListener.cpp
    Obstacle.h
    Obstacle.cpp
    Chicken.h
    Chicken.cpp
    Coin.h
    Coin.cpp
)
target_link_libraries(game
    PRIVATE
    engine
    glfw
    glad
    glm::glm
    box2d
    soloud
)