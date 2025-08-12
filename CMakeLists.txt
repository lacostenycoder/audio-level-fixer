cmake_minimum_required(VERSION 3.15)

project(AudioProcessor VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find JUCE in common locations
set(JUCE_FOUND FALSE)

# Check common JUCE locations
set(JUCE_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE"
    "C:/JUCE"
    "C:/Program Files/JUCE"
    "C:/Users/$ENV{USERNAME}/JUCE"
    "C:/dev/JUCE"
    "C:/tools/JUCE"
)

foreach(JUCE_PATH ${JUCE_PATHS})
    if(EXISTS "${JUCE_PATH}/CMakeLists.txt" OR EXISTS "${JUCE_PATH}/modules")
        add_subdirectory("${JUCE_PATH}" JUCE)
        message(STATUS "Using JUCE from: ${JUCE_PATH}")
        set(JUCE_FOUND TRUE)
        break()
    endif()
endforeach()

# If not found in common locations, try find_package
if(NOT JUCE_FOUND)
    find_package(JUCE CONFIG QUIET)
    if(JUCE_FOUND)
        message(STATUS "Using JUCE from system installation")
    else()
        message(FATAL_ERROR "JUCE not found! Please install JUCE framework.
        
Download JUCE from: https://juce.com/get-juce/download
Extract it to one of these locations:
  - C:/JUCE/
  - C:/Program Files/JUCE/
  - ${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/
  
Or set JUCE_DIR to your JUCE installation path.")
    endif()
endif()

# Create the main application target
juce_add_gui_app(AudioProcessor
    PRODUCT_NAME "Audio Processor"
    COMPANY_NAME "Manus AI"
    VERSION "1.0.0"
    BUNDLE_ID "com.manusai.audioprocessor"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "This app needs access to your microphone to process audio"
)

# Add source files
target_sources(AudioProcessor PRIVATE
    Main.cpp
    MainComponent.cpp
    AudioEngine.cpp
    Compressor.cpp
    Limiter.cpp
    VirtualAudioDevice.cpp
)

# Add platform-specific source files
if(UNIX AND NOT APPLE)
    target_sources(AudioProcessor PRIVATE
        VirtualAudioDevice_Linux.cpp
    )
endif()

# Add header files
target_include_directories(AudioProcessor PRIVATE
    .
)

# Link JUCE modules
target_link_libraries(AudioProcessor PRIVATE
    juce::juce_gui_extra
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_dsp
)

# Generate JuceHeader.h
juce_generate_juce_header(AudioProcessor)

# Compiler definitions
target_compile_definitions(AudioProcessor PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:AudioProcessor,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:AudioProcessor,JUCE_VERSION>"
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_USE_DARK_SPLASH_SCREEN=1
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(AudioProcessor PRIVATE
        JUCE_WASAPI=1
        JUCE_ASIO=0
        JUCE_DIRECTSOUND=1
    )
elseif(APPLE)
    target_compile_definitions(AudioProcessor PRIVATE
        JUCE_COREAUDIO=1
    )
elseif(UNIX)
    target_compile_definitions(AudioProcessor PRIVATE
        JUCE_ALSA=1
        JUCE_JACK=1
    )
    
    # Find and link JACK library
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(JACK REQUIRED jack)
    
    target_include_directories(AudioProcessor PRIVATE ${JACK_INCLUDE_DIRS})
    target_link_libraries(AudioProcessor PRIVATE ${JACK_LIBRARIES})
    target_compile_options(AudioProcessor PRIVATE ${JACK_CFLAGS_OTHER})
endif()

