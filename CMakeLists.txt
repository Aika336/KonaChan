cmake_minimum_required(VERSION 3.11)
project(KonaChan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

add_library(TCP STATIC
    TCP/tcp.cpp
)
target_include_directories(TCP PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/TCP
)

add_library(TCPS STATIC
    TCPS/tcps.cpp
)
target_include_directories(TCPS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/TCPS
)
target_link_libraries(TCPS PUBLIC
    TCP
    OpenSSL::SSL
    OpenSSL::Crypto
)

add_library(HTTP INTERFACE)
target_include_directories(HTTP INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/HTTP
)
target_link_libraries(HTTP INTERFACE
    TCP
    TCPS
)

add_library(KonaChanAPI STATIC
    KonaChanAPI/KonaChan.cpp
)

target_link_directories(KonaChanAPI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/KonaChanAPI    
)

target_link_libraries(KonaChanAPI PUBLIC
    HTTP
)

add_executable(KonaChan
    main.cpp
)
target_link_libraries(KonaChan PRIVATE KonaChanAPI)
