cmake_minimum_required(VERSION 3.1)
project(Evolutionary_Checkers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(SOURCE_LIBS
    lib/gameboard.cpp
    lib/neuralnet.cpp)

set(SOURCE_NETWORK_LIBS
    lib/socket.cpp)

set(SOURCE_NET_TEST
    test_neuralnet.cpp)

set(SOURCE_BOARD_TEST
    test_gameboard.cpp)

set(SOURCE_DB_TEST
    test_db.cpp)

set(SOURCE_CHECKERSTRAINING
    checkerstraining.cpp)

set(SOURCE_CHECKERSCLIENT
    checkersclient.cpp
    lib/socket.cpp)

add_executable(test_neuralnet ${SOURCE_LIBS} ${SOURCE_NET_TEST})
add_executable(test_gameboard ${SOURCE_LIBS} ${SOURCE_BOARD_TEST})
add_executable(test_db ${SOURCE_LIBS} ${SOURCE_DB_TEST})
add_executable(checkerstraining ${SOURCE_LIBS} ${SOURCE_CHECKERSTRAINING})
add_executable(checkersclient ${SOURCE_LIBS} ${SOURCE_CHECKERSCLIENT})
