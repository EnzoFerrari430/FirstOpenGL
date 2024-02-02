cmake_minimum_required(VERSION 3.15)

project(LearnOpenGL)

#可以使用文件夹组织目标
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17) # this does nothing for MSVC, use target_compile_options below
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

#依赖库路径
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

#依赖库 OpenGL GLFW
set(LIBS opengl32 glfw3)

set(CHAPTERS
    1.getting_started
    2.lighting
    #3.model_loading
    #4.advanced_opengl
    #5.advanced_lighting
    #6.pbr
    #7.in_practice
    #8.GUEST_ARTICLES
)

set(1.getting_started
    1.1.hello_window
    2.1.hello_triangle
)

set(2.lighting
    4.1.lighting_maps_diffuse_map
    4.2.lighting_maps_specular_map
    4.4.lighting_maps_exercise4
    5.1.lighting_casters_directional
    5.2.lighting_casters_point
    5.3.lighting_casters_spot
    5.4.lighting_casters_spot_soft
    6.multiple_lights
)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_subdirectory(vendor/imgui)

add_library(STB_IMAGE src/stb_image.cpp)
list(APPEND LIBS STB_IMAGE)
set_property(TARGET STB_IMAGE PROPERTY FOLDER "Dependencies")

add_library(GLAD src/glad.c)
list(APPEND LIBS GLAD)
set_property(TARGET GLAD PROPERTY FOLDER "Dependencies")

add_library(ImGuiBuild src/ImGuiBuild.cpp)
list(APPEND LIBS ImGuiBuild)
set_property(TARGET ImGuiBuild PROPERTY FOLDER "Dependencies")
target_include_directories(ImGuiBuild PUBLIC ${CMAKE_SOURCE_DIR}/vendor/imgui)

target_link_libraries(ImGuiBuild PUBLIC ImGui)

message("LIBS: ${LIBS}")

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()

function(create_project_from_sources chapter demo)
    file(GLOB SOURCE
    "src/${chapter}/${demo}/*.h"
    "src/${chapter}/${demo}/*.cpp"
    "src/${chapter}/${demo}/*.vs"
    "src/${chapter}/${demo}/*.fs"
    "src/${chapter}/${demo}/*.tcs"
    "src/${chapter}/${demo}/*.tes"
    "src/${chapter}/${demo}/*.gs"
    "src/${chapter}/${demo}/*.cs"
    )
    if(demo STREQUAL "")
        SET(replaced "")
        string(REPLACE "/" "_" replaced ${chapter})
        set(NAME ${replaced})
    else()
        set(NAME "${chapter}__${demo}")
    endif()
    add_executable(${NAME} ${SOURCE})
    set_property(TARGET ${NAME} PROPERTY FOLDER "${chapter}")
    target_link_libraries(${NAME} PUBLIC ${LIBS})
    target_include_directories(${NAME} PUBLIC ${CMAKE_SOURCE_DIR}/vendor/imgui)
    if(MSVC)
        target_compile_options(${NAME} PRIVATE /std:c++17 /MP)
        target_link_options(${NAME} PUBLIC /ignore:4099)
    endif(MSVC)
    #设置输出目录
    if(WIN32)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
        set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}/Debug")
    elseif(UNIX AND NOT APPLE)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
    elseif(APPLE)
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${chapter}")
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${chapter}")
        set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${chapter}")
    endif(WIN32)
    # copy shader files to build directory
    file(GLOB SHADERS
             "src/${chapter}/${demo}/*.vs"
             # "src/${chapter}/${demo}/*.frag"
             "src/${chapter}/${demo}/*.fs"
             "src/${chapter}/${demo}/*.tcs"
             "src/${chapter}/${demo}/*.tes"
             "src/${chapter}/${demo}/*.gs"
             "src/${chapter}/${demo}/*.cs"
    )
    foreach(SHADER ${SHADERS})
        if(WIN32)
            # configure_file(${SHADER} "test")
            add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
        elseif(UNIX AND NOT APPLE)
            file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${chapter})
        elseif(APPLE)
            # create symbolic link for *.vs *.fs *.gs
            get_filename_component(SHADERNAME ${SHADER} NAME)
            makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${chapter}/${SHADERNAME} ${NAME})
        endif(WIN32)
    endforeach(SHADER)

    # if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
    if(MSVC)
        configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
    endif()
endfunction()

foreach(CHAPTER ${CHAPTERS})
    message("CHAPTER: ${CHAPTER}")
    foreach(DEMO ${${CHAPTER}})
        message("DEMO: ${DEMO}")
        create_project_from_sources(${CHAPTER} ${DEMO})
    endforeach(DEMO)
endforeach(CHAPTER)


include_directories(${CMAKE_SOURCE_DIR}/includes)
