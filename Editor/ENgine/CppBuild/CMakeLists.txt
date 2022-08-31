cmake_minimum_required(VERSION 2.8.6)
project (gameplay)
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_STANDARD 17)

add_definitions("-DOAK_EDITOR")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /wd4251 /wd4275")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /wd4251 /wd4275")

include_directories ("$EDITOR_DIR/ENgine/CppBuild/include/Libs/vjson"
					 "$EDITOR_DIR/ENgine/CppBuild/include/Libs/eastl/include"
					 "$EDITOR_DIR/ENgine/CppBuild/include/Libs/eastl/include/Common"
					 "$EDITOR_DIR/ENgine/CppBuild/include/Libs/fmod/include"
					 "$EDITOR_DIR/ENgine/CppBuild/include/Libs/physx/include"
					 "$EDITOR_DIR/ENgine/CppBuild/include/ENgine/Support/PugiXml"
					 "$EDITOR_DIR/ENgine/CppBuild/include/ENgine/Support/SPARK"
					 "$EDITOR_DIR/ENgine/CppBuild/include/ENgine")

file (GLOB_RECURSE SOURCES "../*.cpp" "../*.h" "$EDITOR_DIR/ENgine/CppBuild/Gameplay.cpp")

add_library(gameplay SHARED ${SOURCES})

set_target_properties(gameplay PROPERTIES OUTPUT_NAME "gameplay_Release")
set_target_properties(gameplay PROPERTIES DEBUG_OUTPUT_NAME "gameplay_Debug")
set_target_properties(gameplay PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/../Binaries/Win64>)

target_link_options(gameplay PUBLIC /DEBUG:FASTLINK)

target_link_libraries(gameplay optimized "$EDITOR_DIR/ENgine/CppBuild/lib/eastl.lib" optimized "$EDITOR_DIR/ENgine/CppBuild/lib/Oak.lib")
target_link_libraries(gameplay debug "$EDITOR_DIR/ENgine/CppBuild/lib/eastl_Debug.lib" debug "$EDITOR_DIR/ENgine/CppBuild/lib/Oak_Debug.lib")
