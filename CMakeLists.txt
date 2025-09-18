cmake_minimum_required(VERSION 3.16)

#-------------------------------------------------------

file(READ ${CMAKE_CURRENT_LIST_DIR}/assets/data/info.json JSON_CONTENT)

# Extract project name
string(REGEX MATCH "\"project\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_NAME ${JSON_CONTENT})
set(JSON_PROJECT ${CMAKE_MATCH_1})

# Extract company name
string(REGEX MATCH "\"company\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_COMPANY ${JSON_CONTENT})
set(JSON_COMPANY ${CMAKE_MATCH_1})

# Extract version
string(REGEX MATCH "\"version\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_VERSION ${JSON_CONTENT})
set(JSON_VERSION ${CMAKE_MATCH_1})

string(REGEX MATCH "\"build-mode\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_BUILD_MODE ${JSON_CONTENT})
set(JSON_BUILD_MODE ${CMAKE_MATCH_1})

# Extract icon-path
string(REGEX MATCH "\"icon-path\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_ICON_PATH ${JSON_CONTENT})
set(JSON_ICON_PATH ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_MATCH_1})

# Extract assets-path
string(REGEX MATCH "\"assets-path\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_ASSETS_PATH ${JSON_CONTENT})
set(JSON_ASSETS_PATH ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_MATCH_1})

# Extract resources-path
string(REGEX MATCH "\"resources-path\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_RESOURCES_PATH ${JSON_CONTENT})
set(JSON_RESOURCES_PATH ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_MATCH_1})

# Extract include-path
string(REGEX MATCH "\"include-path\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_INCLUDE_PATH ${JSON_CONTENT})
set(JSON_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_MATCH_1})

# Extract source-path
string(REGEX MATCH "\"source-path\"[ \t]*:[ \t]*\"([^\"]+)\"" _ MATCH_SOURCE_PATH ${JSON_CONTENT})
set(JSON_SOURCE_PATH ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_MATCH_1})

#-------------------------------------------------------

# execute_process(
#     COMMAND git submodule update --init --recursive
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR} CACHE INTERNAL "")

# Declare the project
project(${JSON_PROJECT} VERSION ${JSON_VERSION} LANGUAGES CXX)

if(UNIX AND NOT APPLE) # Linux or BSD
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

if(APPLE OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "") # Disable shared builds on platforms where it does not make sense to use them
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP) # parallelize each target, unless Ninja is the generator
    endif()
endif()

# Set the name of the executable
set(EXECUTABLE_NAME ${JSON_PROJECT})

# Create an executable or a shared library based on the platform and add our sources to it
if(ANDROID)
    # The SDL java code is hardcoded to load libmain.so on android, so we need to change EXECUTABLE_NAME
    set(EXECUTABLE_NAME main)
    add_library(${EXECUTABLE_NAME} SHARED)
elseif(WIN32)
    add_executable(${EXECUTABLE_NAME} WIN32)
    target_sources(${EXECUTABLE_NAME} PRIVATE resources/appicon.rc)
else()
    add_executable(${EXECUTABLE_NAME})
endif()

if(JSON_BUILD_MODE STREQUAL "Debug")
    add_definitions(-DDEBUG_MODE)
elseif(JSON_BUILD_MODE STREQUAL "Release")
    add_definitions(-DRELEASE_MODE)
endif()


target_include_directories(${EXECUTABLE_NAME} PRIVATE ${JSON_INCLUDE_PATH})

# Automatically include all .cpp files in src/
file(GLOB_RECURSE SOURCES "${JSON_SOURCE_PATH}/*.cpp")

# Add manually extra files (e.g., platform-specific or non-.cpp files)
if(ANDROID)
    set(EXTRA_SOURCES "") # Android cannot compile .storyboard
else()
    set(EXTRA_SOURCES
        ${JSON_RESOURCES_PATH}/iosLaunchScreen.storyboard
    )
endif()

# Combine them
list(APPEND SOURCES ${EXTRA_SOURCES})

# Remove main.cpp if it’s already in the list
list(REMOVE_ITEM SOURCES ${JSON_SOURCE_PATH}/main.cpp)

# Ensure main.cpp is first (important for Android)
list(INSERT SOURCES 0 ${JSON_SOURCE_PATH}/main.cpp)

# Add them to your target
target_sources(${EXECUTABLE_NAME} PRIVATE ${SOURCES})

# What is iosLaunchScreen.storyboard? This file describes what Apple's mobile platforms
# should show the user while the application is starting up. If you don't include one,
# then you get placed in a compatibility mode that does not allow HighDPI.
# This file is referenced inside Info.plist.in, where it is marked as the launch screen file.
# It is also ignored on non-Apple platforms. 

# To get an app icon on Apple platforms, add it to your executable.
# Afterward, the image file in Info.plist.in.
if(APPLE)
    target_sources(${EXECUTABLE_NAME} PRIVATE ${JSON_ICON_PATH})
endif()

# Set C++ version
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_23)

# on Web targets, we need CMake to generate a HTML webpage. 
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js" CACHE INTERNAL "")

    target_link_options(${EXECUTABLE_NAME} PRIVATE
        -lidbfs.js
        -sFORCE_FILESYSTEM=1
        -sASYNCIFY
    )
endif()

# Configure SDL by calling its CMake file.
# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# If you don't want SDL_ttf, then remove this section.
set(SDLTTF_VENDORED ON) # tell SDL_ttf to build its own dependencies
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)

# SDL_mixer (used for playing audio)
set(SDLMIXER_MIDI_NATIVE OFF) # disable formats we don't use to make the build faster and smaller. Also some of these don't work on all platforms so you'll need to do some experimentation.
set(SDLMIXER_GME OFF)
set(SDLMIXER_WAVPACK OFF)
set(SDLMIXER_MOD OFF)
set(SDLMIXER_OPUS OFF)
set(SDLMIXER_MP3_MPG123 OFF)
set(SDLMIXER_VORBIS_VORBISFILE OFF)
set(SDLMIXER_FLAC_LIBFLAC OFF)
set(SDLMIXER_JACK OFF)
set(SDLMIXER_VENDORED ON) # tell SDL_mixer to build its own dependencies
add_subdirectory(SDL_mixer EXCLUDE_FROM_ALL)

# SDL_image (used for loading various image formats)
set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_AVIF OFF) # disable formats we don't use to make the build faster and smaller.
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_JPEG OFF)
set(SDLIMAGE_WEBP OFF)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)

# Link SDL to our executable. This also makes its include directory available to us. 
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
    SDL3_ttf::SDL3_ttf # remove if you are not using SDL_ttf
    SDL3_mixer::SDL3_mixer # remove if you are not using SDL_mixer
    SDL3_image::SDL3_image # remove if you are not using SDL_image
    SDL3::SDL3 # If using satelite libraries, SDL must be the last item in the list. 
)

# SDL_Image bug: https://github.com/libsdl-org/SDL_image/issues/506
if(APPLE AND NOT BUILD_SHARED_LIBS)
    find_library(IO_LIB ImageIO REQUIRED)
    find_library(CS_LIB CoreServices REQUIRED)
    find_library(CT_LIB CoreText REQUIRED)
    find_library(CG_LIB CoreGraphics REQUIRED)
    find_library(CF_LIB CoreFoundation REQUIRED)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CF_LIB} ${CT_LIB} ${IO_LIB} ${CS_LIB} ${CG_LIB})
endif()

# Dealing with assets
# We have some non-code resources that our application needs in order to work. How we deal with those differs per platform.
set(ASSETS_DIR ${JSON_ASSETS_PATH})

# Collect all files in the assets folder (recursively)
file(GLOB_RECURSE ASSETS ${JSON_ASSETS_PATH}/*)

if(APPLE)
    # On Apple, put everything into the app bundle Resources/assets folder
    foreach(asset ${ASSETS})
        file(RELATIVE_PATH relpath ${JSON_ASSETS_PATH} ${asset})
        set(relpath "assets/${relpath}") # <--- put everything in assets/
        target_sources(${EXECUTABLE_NAME} PRIVATE ${asset})
        set_property(SOURCE ${asset} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endforeach()

elseif(EMSCRIPTEN)
    # Preload all assets into the web build under /assets
    foreach(asset ${ASSETS})
        file(RELATIVE_PATH relpath ${JSON_ASSETS_PATH} ${asset})
        set(relpath "assets/${relpath}") # <--- put everything in assets/
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE
            "--preload-file \"${asset}@/${relpath}\""
        )
    endforeach()

else()
    if(ANDROID)
        if(NOT MOBILE_ASSETS_DIR)
            message(FATAL_ERROR "Building for Android, but MOBILE_ASSETS_DIR is not set")
        endif()
        file(MAKE_DIRECTORY "${MOBILE_ASSETS_DIR}/assets")
    endif()

    foreach(asset ${ASSETS})
        file(RELATIVE_PATH relpath ${JSON_ASSETS_PATH} ${asset})
        set(relpath "assets/${relpath}") # <--- put everything in assets/
        get_filename_component(asset_dir "${relpath}" DIRECTORY)

        if(ANDROID)
            set(outname "${MOBILE_ASSETS_DIR}/${relpath}")
        else()
            set(outname "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${relpath}")
        endif()

        # Ensure the directory exists
        add_custom_command(POST_BUILD
            TARGET ${EXECUTABLE_NAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/${asset_dir}"
        )

        # Copy the file
        add_custom_command(POST_BUILD
            TARGET ${EXECUTABLE_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${asset} "${outname}"
            DEPENDS ${asset}
        )
    endforeach()
endif()

# set some extra configs for each platform
set(RESOURCE_FILES ${JSON_ICON_PATH})
if(IOS)
    set(RESOURCE_FILES ${JSON_RESOURCES_PATH}/iosLaunchScreen.storyboard;${JSON_ICON_PATH}) # make the splash screen show up on iOS
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    # On macOS, make a proper .app bundle instead of a bare executable
    MACOSX_BUNDLE TRUE
    # Set the Info.plist file for Apple Mobile platforms. Without this file, your app
    # will not launch. 
    MACOSX_BUNDLE_INFO_PLIST ${JSON_RESOURCES_PATH}/Info.plist.in

    # in Xcode, create a Scheme in the schemes dropdown for the app.
    XCODE_GENERATE_SCHEME TRUE
    # Identification for Xcode
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
    XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
    RESOURCE "${RESOURCE_FILES}"
)

# on Visual Studio, set our app as the default project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

# On macOS Platforms, ensure that the bundle is valid for distribution by calling fixup_bundle.
# note that fixup_bundle does not work on iOS, so you will want to use static libraries 
# or manually copy dylibs and set rpaths
if(APPLE)
    # tell Install about the target, otherwise fixup won't know about the transitive dependencies
    install(TARGETS ${EXECUTABLE_NAME}
        BUNDLE DESTINATION ./install COMPONENT Runtime
        RUNTIME DESTINATION ./install/bin COMPONENT Runtime
    )

    set(DEP_DIR "${CMAKE_BINARY_DIR}") # where to look for dependencies when fixing up
    INSTALL(CODE
        "include(BundleUtilities)
        fixup_bundle(\"$<TARGET_BUNDLE_DIR:${EXECUTABLE_NAME}>\" \"\" \"${DEP_DIR}\")
        "
    )
    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
endif()
