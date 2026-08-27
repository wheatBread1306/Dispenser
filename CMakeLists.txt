message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 3.28)

# LANGUAGES に C を追加
project(Dispenser VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# FetchContentを使用してJUCEを自動的に取得
include(FetchContent)
FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 9.0.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(JUCE)

# オーディオプラグイン・ターゲットの追加
juce_add_plugin(Dispenser
        VERSION "1.0.0"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_NAME "DISPENSER"
        COMPANY_NAME "WheatBreadPlugins"
        COMPANY_WEBSITE "https://wheadbread.dev/"
        FORMATS VST3 Standalone
        VST3_CATEGORIES EFFECTS
)

# ソースファイルの追加
target_sources(Dispenser PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h

        Source/Parameters/PluginParameters.h

        Source/DSP/APFCore.cpp
        Source/DSP/APFCore.h
        Source/DSP/CascadeAPF.cpp
        Source/DSP/CascadeAPF.h
        Source/DSP/Manager.cpp
        Source/DSP/Manager.h
)

# リンクするモジュール
target_link_libraries(Dispenser PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
)

# コンパイルオプションの設定
juce_generate_juce_header(Dispenser)

# コンパイル定義（JUCE OPTIONS）と最適化フラグ
target_compile_definitions(Dispenser PUBLIC
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_NAMESPACE_JUCE=1
)

# コンパイラフラグ
target_compile_options(Dispenser PRIVATE
        $<$<CXX_COMPILER_ID:Clang,MSVC>:/utf-8 /arch:AVX2 /external:W0 /W0>
)

target_compile_options(Dispenser PRIVATE
        $<$<CONFIG:Release>:
        $<$<CXX_COMPILER_ID:Clang,MSVC>:/fp:fast /Ox /Ob2 /Oi /Ot /GS- /Gy /Zc:rvalueCast>
        $<$<CXX_COMPILER_ID:Clang>:
        -w
        -flto
        -O3
        >
        >
)

target_link_options(Dispenser PRIVATE
        $<$<CONFIG:Release>:
        $<$<CXX_COMPILER_ID:Clang>:-fuse-ld=lld>
        $<$<CXX_COMPILER_ID:Clang,MSVC>:/INCREMENTAL:NO /OPT:REF /OPT:ICF /LTCG>
        >
)

if(MSVC)
    target_compile_options(Dispenser PRIVATE
            $<$<CONFIG:Debug>:
            /MP
            /Zi
            /Od
            /GS-
            /Gm-
            /fsanitize=address
            >
    )

    target_link_options(Dispenser PRIVATE
            $<$<CONFIG:Debug>:
            /INCREMENTAL
            /OPT:NOREF
            /OPT:NOICF
            /fsanitize=address
            >
    )

    target_compile_definitions(Dispenser PRIVATE
            _DISABLE_OPTIONAL_ANNOTATION=1
            _DISABLE_VECTOR_ANNOTATION=1
            _DISABLE_STRING_ANNOTATION=1
    )

endif()

# バイナリリソース（WAVファイル）の追加設定
#juce_add_binary_data(DispenserBinaryData
#        SOURCES
#)

#target_link_libraries(Dispenser PRIVATE DispenserBinaryData)