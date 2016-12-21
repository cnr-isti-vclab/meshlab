project(Grenaille)
cmake_minimum_required(VERSION 2.8)


FILE ( GLOB GRENAILLE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h
)

FILE ( GLOB GRENAILLE_IMPL
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.hpp
)

add_library (Grenaille OBJECT ${GRENAILLE_HEADERS} ${GRENAILLE_IMPL})
