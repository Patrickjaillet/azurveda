add_library(VedaMachineOGL STATIC
    OGLMachine.cpp
)

target_include_directories(VedaMachineOGL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaMachineOGL PUBLIC Veda opengl32)
