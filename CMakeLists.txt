cmake_minimum_required(VERSION 3.23)
project(tecnospire)

set(CMAKE_CXX_STANDARD 20)

include_directories(I:/onedrive/SDL2/include libs)
link_directories(I:/onedrive/SDL2/lib/x64 libs)

#lua54.dll needs to be copied to windows/system32 folder
add_library(Lua STATIC IMPORTED GLOBAL)
set_target_properties(Lua PROPERTIES
        IMPORTED_LOCATION "I:/onedrive/lua4/liblua54.a"
        INTERFACE_INCLUDE_DIRECTORIES "I:/onedrive/lua4/include"
        )


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/*.cpp)


add_executable(${PROJECT_NAME}  ${SRC_FILES} src/GUI/Popup.h src/GUI/Popup.cpp src/base64/base64.cpp src/base64/base64.h src/Audio/Audio.cpp)

#looks for a file with .lib extension
target_link_libraries(${PROJECT_NAME} SDL2main SDL2 SDL2_ttf SDL2_image SDL2_mixer Lua)

#AssetManager
add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
        )
add_dependencies(${PROJECT_NAME} copy_assets)