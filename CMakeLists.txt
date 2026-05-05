cmake_minimum_required(VERSION 3.28)
project(gitpta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    gitpta
    main.cpp
    init.cpp
    add.cpp
    commit.cpp
    )

