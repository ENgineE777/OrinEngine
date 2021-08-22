cmake_minimum_required(VERSION 2.8.6)
project (gameplay)
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_STANDARD 17)

add_definitions("-DOAK_EDITOR")

include_directories ("../../../../Libs/vjson"
					 "../../../../Libs/eastl/include"
					 "../../../../Libs/eastl/include/Common"
					 "../../../../Libs/fmod/include"
					 "../../../../Libs/physx/include"
					 "../../../../ENgine/Support/PugiXml"
					 "../../../../ENgine/Support/SPARK"
					 "../../../../ENgine")

file (GLOB_RECURSE SOURCES "../*.cpp" "../*.h" "../../../ENgine/CppBuild/Gameplay.cpp")

add_library(gameplay SHARED ${SOURCES})

target_link_libraries(gameplay optimized "../../../ENgine/CppBuild/lib/eastl" optimized "../../../ENgine/CppBuild/lib/Oak")
target_link_libraries(gameplay debug "../../../ENgine/CppBuild/lib/eastl_Debug" debug "../../../ENgine/CppBuild/lib/Oak_Debug")