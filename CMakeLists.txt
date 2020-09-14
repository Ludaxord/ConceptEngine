cmake_minimum_required(VERSION 3.17)
project(ConceptEngine)

set(CMAKE_CXX_STANDARD 14)

add_executable(ConceptEngine main.cpp
#        api/platform/apple/mac/opengl/ConceptEngine/MacConceptEngineOpenGL/MacConceptEngineOpenGL/CEMacOpenGL.cpp
#        api/platform/apple/mac/opengl/ConceptEngine/MacConceptEngineOpenGL/MacConceptEngineOpenGL/CEMacOpenGL.h
        api/platform/CEPlatformInit.cpp
        api/platform/CEPlatformInit.h
        )

add_definitions(-DBOOST_ALL_NO_LIB) # disabling of auto filling boost libs names to versioned
