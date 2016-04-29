cmake_minimum_required (VERSION 3.1)
project (Quaketrace)
string(TOLOWER ${CMAKE_PROJECT_NAME} EXEC_FILE)

set(SHOW_GUI 0 CACHE BOOL "Show output on screen while rendering")
if (SHOW_GUI)
    add_definitions(-DSHOW_GUI=1)

    if (WIN32)
        set(SDL2_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/sdl-win")
        set(SDL2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdl-win/include")
        if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
            set(SDL2_LIBRARY_PATH "${SDL2_ROOT}/lib/x86")
        else ()
            set(SDL2_LIBRARY_PATH "${SDL2_ROOT}/lib/x64")
        endif ()
        set(SDL2_SHARED_DLL "${SDL2_LIBRARY_PATH}/SDL2.dll")
        find_library(SDL2_LIBRARY_MAIN NAMES SDL2main PATHS "${SDL2_LIBRARY_PATH}")
        find_library(SDL2_LIBRARY NAMES SDL2 PATHS "${SDL2_LIBRARY_PATH}")
        file(COPY ${SDL2_SHARED_DLL} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}" NO_SOURCE_PERMISSIONS)
    else()
        set(SDL2_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/sdl-osx")
        set(SDL2_LIBRARY_PATH ${SDL2_ROOT})
        find_library(COCOA_LIBRARY Cocoa)
        find_library(OPENGL_LIBRARY OpenGL)
        find_library(SDL2_LIBRARY SDL2)
        find_path(SDL2_INCLUDE_DIR "SDL.h" PATHS ${SDL2_LIBRARY})
    endif()
    include_directories (${SDL2_INCLUDE_DIR})
else()
    add_definitions(-DSHOW_GUI=0)
endif()

if(WIN32)
    add_definitions(-DTARGET_WIN32=1)
else()
    add_definitions(-DTARGET_WIN32=0)
endif()

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Assets to be bundled in the executable
set(ASSETS
	assets/palette.lmp
	assets/font.bmp
	assets/icon.bmp
)

# Generate source files
set (BASE_SOURCE_FILES
	main.cpp
    Texture.hpp
    Texture.cpp
	Geometry.hpp
	AssetHelper.hpp
	AssetHelper.cpp
	Scene.cpp
	Scene.hpp
	Color.hpp
	Assert.hpp
	Ray.hpp
	Collision3D.cpp
	Collision3D.hpp
    BspLoader.cpp
    BspLoader.hpp
    BspEntity.cpp
    BspEntity.hpp
    Logger.hpp
    Lighting.hpp
    Lighting.cpp
    Camera.hpp
    Camera.cpp
    RayTracer.hpp
    RayTracer.cpp
    BackgroundTracer.hpp
    BackgroundTracer.cpp
)

set(UTIL_SOURCE_FILES
    Util.hpp
    ArrayView.hpp
    StringTool.cpp
    StringTool.hpp
    Random.hpp
    BinaryWriter.hpp
    BinaryWriter.cpp
    File.hpp
    CommandLine.cpp
    CommandLine.hpp
    BreakPoint.hpp
)

set(IMAGE_SOURCE_FILES
    Image.hpp
    Image.cpp
    Targa.hpp
    Targa.cpp
)

set(THREADING_SOURCE_FILES
    Scheduler.cpp
    Scheduler.hpp
    Task.hpp
)

set (MATH_SOURCE_FILES
	Math.cpp
	Math.hpp
	Vec2.hpp
	Vec3.hpp
	Vec4.hpp
	Mat22.hpp
	Mat33.hpp
	Mat44.hpp
)

set (FRONTEND_SOURCE_FILES
    AppConfig.hpp
    AppConfig.cpp
    Common.hpp
    Common.cpp
)

set (CONSOLE_SOURCE_FILES
    Console.hpp
    Console.cpp
)

set (GUI_SOURCE_FILES
    GUI.hpp
    GUI.cpp
    Font.cpp
    Font.hpp
    FrameBuffer.cpp
    FrameBuffer.hpp
    GraphicTool.cpp
    GraphicTool.hpp
)

source_group("Source" FILES ${BASE_SOURCE_FILES})
source_group("Source\\Math" FILES ${MATH_SOURCE_FILES})
source_group("Source\\Util" FILES ${UTIL_SOURCE_FILES})
source_group("Source\\Threading" FILES ${THREADING_SOURCE_FILES})
source_group("Source\\Image" FILES ${IMAGE_SOURCE_FILES})
source_group("Source\\Frontend" FILES ${FRONTEND_SOURCE_FILES} ${GUI_SOURCE_FILES} ${CONSOLE_SOURCE_FILES})

set (APP_SOURCE_FILES
	${BASE_SOURCE_FILES}
	${MATH_SOURCE_FILES}
    ${UTIL_SOURCE_FILES}
    ${THREADING_SOURCE_FILES}
    ${IMAGE_SOURCE_FILES}
    ${FRONTEND_SOURCE_FILES}
)

if (SHOW_GUI)
    list(APPEND APP_SOURCE_FILES ${GUI_SOURCE_FILES})
else()
    list(APPEND APP_SOURCE_FILES ${CONSOLE_SOURCE_FILES})
endif()

foreach(f IN ITEMS ${APP_SOURCE_FILES})
	set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
	if(NOT EXISTS ${SOURCE_FILE})
		message(STATUS "Creating non-existing source file: ${f}")
		file(WRITE ${SOURCE_FILE} "")
	endif()
endforeach()

if (WIN32)
    if (SHOW_GUI)
        add_executable (${EXEC_FILE} WIN32 ${APP_SOURCE_FILES})
        target_link_libraries(${EXEC_FILE} ${SDL2_LIBRARY_MAIN} ${SDL2_LIBRARY})
    else()
        add_executable (${EXEC_FILE} ${APP_SOURCE_FILES})
    endif()
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_executable (${EXEC_FILE} ${APP_SOURCE_FILES})
    if (SHOW_GUI)
        target_link_libraries(${EXEC_FILE} ${SDL2_LIBRARY} ${COCOA_LIBRARY} ${OPENGL_LIBRARY})
    endif()
endif()

set_property(TARGET ${EXEC_FILE} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${EXEC_FILE} PROPERTY CXX_STANDARD_REQUIRED ON)

find_package(PythonInterp REQUIRED)
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bundled.inc"
	COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/assets/bundle.py" "${CMAKE_CURRENT_BINARY_DIR}/bundled.inc"
	DEPENDS ${ASSETS}
)
add_custom_target(bundle_generator DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/bundled.inc")
add_dependencies(${EXEC_FILE} bundle_generator)
