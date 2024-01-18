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
    #2.lighting
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


add_library(glad src/glad.c)
list(APPEND LIBS glad)
set_property(TARGET glad PROPERTY FOLDER "Dependencies")

message("LIBS: ${LIBS}")

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
    add_executable(${demo} ${SOURCE})
    set_property(TARGET ${demo} PROPERTY FOLDER "${chapter}")
    target_link_libraries(${demo} PUBLIC ${LIBS})
endfunction()

foreach(CHAPTER ${CHAPTERS})
    message("CHAPTER: ${CHAPTER}")
    foreach(DEMO ${${CHAPTER}})
        message("DEMO: ${DEMO}")
        create_project_from_sources(${CHAPTER} ${DEMO})
    endforeach(DEMO)
endforeach(CHAPTER)


include_directories(${CMAKE_SOURCE_DIR}/includes)
