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

link_libraries("../../../eastl_Debug"
	       "../../../Oak_Debug")

add_library(gameplay SHARED ${SOURCES})