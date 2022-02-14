include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SRC_FILES *.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_library(render STATIC ${SRC_FILES})
target_compile_features(render PRIVATE cxx_std_17)
target_link_libraries(render stb Eigen::Eigen OpenMP::OpenMP_CXX)

add_executable(main main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(
        main PRIVATE
        render
)