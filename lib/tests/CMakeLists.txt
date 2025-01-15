link_libraries(${PROJECT_NAME})

set(UNITTEST_SOURCES
    unittests/DrObjectTest.cpp
)

add_executable(unittest ${UNITTEST_SOURCES})

set_property(TARGET ${tests} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${tests} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${tests} PROPERTY CXX_EXTENSIONS OFF)