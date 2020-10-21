project(Vitelotte)
cmake_minimum_required(VERSION 2.8)


FILE ( GLOB VITELOTTE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.h
)

FILE ( GLOB VITELOTTE_IMPL
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.hpp
)

add_library (Vitelotte OBJECT ${VITELOTTE_HEADERS} ${VITELOTTE_IMPL})
