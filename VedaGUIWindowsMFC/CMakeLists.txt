set(CMAKE_MFC_FLAG 2)

add_executable(VedaGUIWindowsMFC WIN32
    ChildFrm.cpp
    Dialog_AbstractPackSerializable.cpp
    Dialog_PackDynamicType.cpp
    Dialog_PackFloat.cpp
    Dialog_PackFloatByte_Limits.cpp
    Dialog_PackListDynamic.cpp
    Dialog_PackLong_WithLimits.cpp
    Dialog_PackReference.cpp
    Dialog_PackResource.cpp
    Dialog_PackSerializable.cpp
    Dialog_PackString.cpp
    Dialog_PackStringMultiple.cpp
    Dialog_PackStruct.cpp
    Dialog_PackTreeCell.cpp
    Dialog_PackULong.cpp
    Dialog_PackULongEnums.cpp
    Dialog_PackULongFlags.cpp
    DialogEditWithReturn.cpp
    FormViewBaseObject.cpp
    LeftView.cpp
    MainFrm.cpp
    OpenGLWnd.cpp
    PreviewMachine.cpp
    PreviewManagerView.cpp
    stdafx.cpp
    VedaDemoOGLMfcGui.cpp
    VedaDemoOGLMfcGuiDoc.cpp
    XSplitterWndFixed.cpp
    VedaDemoOGLMfcGui.rc
)

target_include_directories(VedaGUIWindowsMFC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(VedaGUIWindowsMFC PRIVATE _AFXDLL)
target_precompile_headers(VedaGUIWindowsMFC PRIVATE stdafx.h)

# res/VedaDemoOGLMfcGui.manifest declares Windows 10/11-only OS compatibility and
# per-monitor DPI awareness; merged into the linker-generated default manifest
# rather than relying on the auto-generated one alone.
target_link_options(VedaGUIWindowsMFC PRIVATE
    "/MANIFEST:EMBED"
    "/MANIFESTINPUT:${CMAKE_CURRENT_SOURCE_DIR}/res/VedaDemoOGLMfcGui.manifest"
)

target_link_libraries(VedaGUIWindowsMFC PRIVATE
    Veda
    VedaLibMath
    VedaLibImage
    VedaLibDemo
    VedaMachineOGL
    VedaMachineOGLWinDxSound
    VedaLibSoundMP3
    glu32
    odbc32
    odbccp32
)
