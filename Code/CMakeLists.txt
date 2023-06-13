cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 14)

project(Impltment-STL)

#FILE(GLOB SOURCE_1 "${CMAKE_SOURCE_DIR}/vector_test/nonPOD_vector_test.cpp")
#FILE(GLOB SOURCE_1 "${CMAKE_SOURCE_DIR}/vector_test/vector_test.cpp")
#FILE(GLOB SOURCE_1 "${CMAKE_SOURCE_DIR}/code_test/list_iterator.cpp")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include ")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/memory ")


add_executable(heap_test code_test/hash_table_POD.cpp include/stl_hash_fun.h include/my_hash_set.h include/my_hash_map.h)