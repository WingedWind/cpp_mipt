find_package(Boost REQUIRED COMPONENTS dll)

add_library(shared_v1 SHARED shared_v1.cpp)
add_library(shared_v2 SHARED shared_v2.cpp)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE Boost::dll)

set_target_properties(app shared_v1 shared_v2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(shared_v1 shared_v2 PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
