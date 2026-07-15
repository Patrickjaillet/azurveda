add_library(VedaLibSoundMP3 STATIC
    MP3SoundObject.cpp
    RegisterLibSoundMP3.cpp
    bit.c
    decoder.c
    fixed.c
    frame.c
    huffman.c
    layer12.c
    layer3.c
    stream.c
    synth.c
    timer.c
)

# Vendored libmad decoder: relax /W4 /WX to /W1 /WX- (third-party code, not held
# to AzurVeda's warning bar; see root CMakeLists.txt).
set_source_files_properties(
    bit.c decoder.c fixed.c frame.c huffman.c layer12.c layer3.c stream.c synth.c timer.c
    PROPERTIES COMPILE_OPTIONS "/W1;/WX-"
)

target_include_directories(VedaLibSoundMP3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VedaLibSoundMP3 PUBLIC Veda VedaLibMath VedaLibImage)
target_compile_definitions(VedaLibSoundMP3 PRIVATE
    FPM_64BIT
    HAVE_CONFIG_H
    ASO_ZEROCHECK
    HAVE_ASSERT_H
    HAVE_FCNTL_H
)
