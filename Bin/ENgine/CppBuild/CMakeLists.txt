cmake_minimum_required(VERSION 2.8.6)
project (gameplay)
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_STANDARD 17)

add_definitions("-DOAK_EDITOR" "-DOAK_EXPORTING")

include_directories ("../../../../Libs/vjson"
					 "../../../../Libs/eastl/include"
					 "../../../../Libs/eastl/include/Common"
					 "../../../../ENgine")

file (GLOB_RECURSE SOURCES "../*.cpp" "../*.h" "../../../ENgine/CppBuild/Gameplay.cpp")

add_library(gameplay SHARED ${SOURCES})

target_link_libraries(gameplay optimized "../../../eastl" optimized "../../../Oak")
target_link_libraries(gameplay debug "../../../eastl_Debug" debug "../../../Oak_Debug")