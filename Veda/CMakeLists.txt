add_library(Veda STATIC
    BaseContext.cpp
    BaseObject.cpp
    BaseType.cpp
    ClassNode.cpp
    InterfacePrefObject.cpp
    MediaAccess.cpp
    MediaTimePattern.cpp
    NamedObject.cpp
    PackDynamicType.cpp
    PackFloat.cpp
    PackFloatByte_Color.cpp
    PackFloatByte_Limits.cpp
    PackFloat_FixedPoint.cpp
    PackList.cpp
    PackList_TimeBlockTrack.cpp
    PackList_TimeTrack.cpp
    PackLong.cpp
    PackLong_WithLimits.cpp
    PackObjectReference.cpp
    PackResource.cpp
    PackString.cpp
    PackString_MultipleLines.cpp
    PackStruct.cpp
    PackTreeCell.cpp
    PackULong.cpp
    PackULong_Enums.cpp
    PackULong_Flags.cpp
    PackULong_RandomSeed.cpp
    TextLocalized.cpp
    VirtualMachine.cpp
    VirtualMedia.cpp
)

target_include_directories(Veda PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Veda PUBLIC AzurVedaVersion)
target_compile_definitions(Veda PUBLIC WIN32 $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG> _ENGINE_EDITABLE_)
