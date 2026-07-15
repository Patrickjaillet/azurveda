add_library(VedaLibMath STATIC
    EquationGamma.cpp
    EquationList.cpp
    EquationSinus3D.cpp
    EquationSoundSignal.cpp
    EquationSpline_X.cpp
    EquationSpline_XY.cpp
    EquationSpline_XYZ.cpp
    PackDynamicFloat.cpp
    RegisterLibMath.cpp
    VirtualEquation.cpp
    VirtualEquationSpline.cpp
)

target_include_directories(VedaLibMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLibMath PUBLIC Veda)
