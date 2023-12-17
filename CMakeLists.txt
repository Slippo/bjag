cmake_minimum_required(VERSION 2.6)

# Name of project
set(PROJ_NAME "3501_Leagues_Under_the_Sea")
project(${PROJ_NAME})

# Specify project files: header files and source files
set(HDRS
      camera.h composite_node.h  game.h  resource.h resource_manager.h scene_graph.h scene_node.h manipulator.h game_collision.h imgui/imgui.h imgui/imgui_impl_glfw.h imgui/imgui_impl_opengl3.h imgui/imgui_impl_opengl3_loader.h imgui/imgui_internal.h imgui/imstb_rectpack.h imgui/imstb_textedit.h imgui/imstb_truetype.h model_loader.h
)
 
set(SRCS
     camera.cpp composite_node.cpp  game.cpp main.cpp  resource.cpp resource_manager.cpp scene_graph.cpp scene_node.cpp manipulator.cpp screen_space_vp.glsl screen_space_fp.glsl kelp_material_vp.glsl kelp_material_fp.glsl material_vp.glsl material_fp.glsl game_collision.cpp environment_fp.glsl environment_gp.glsl environment_vp.glsl combined_fp.glsl combined_vp.glsl particle_vent_vp.glsl particle_vent_gp.glsl particle_vent_fp.glsl particle_bubbles_vp.glsl particle_bubbles_gp.glsl particle_bubbles_fp.glsl star_fp.glsl star_gp.glsl star_vp.glsl imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
)

set(IRRKLANG_DLL_PATH irrKlang.dll)

#set(IKPMP3_DLL_PATH ikpMP3.dll)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Add path name to configuration file
#configure_file(path_config.h.in path_config.h)
#file(COPY ${IRRKLANG_DLL_PATH} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
#file(COPY ${IKPMP3_DLL_PATH} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# Add executable based on the header and source files
add_executable(${PROJ_NAME} ${HDRS} ${SRCS})

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})

# ImGui library
include_directories(${CMAKE_CURRENT_LIST_DIR}/imgui)

# Other libraries needed
set(LIBRARY_PATH C:/Software/Libraries)
include_directories(${LIBRARY_PATH}/include)

if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY glfw)
    find_library(SOIL_LIBRARY SOIL)
    find_library(IRRKLANG_LIBRARY IRRKLANG)
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
    find_library(IRRKLANG_LIBRARY IRRKLANG HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)


target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})
target_link_libraries(${PROJ_NAME} ${IRRKLANG_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)

