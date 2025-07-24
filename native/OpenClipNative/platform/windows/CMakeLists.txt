target_compile_definitions(OpenClipNative PRIVATE
    _WIN32_WINNT=0x0A00
)

target_sources(OpenClipNative PRIVATE
    WASAPIAudio.cpp
)