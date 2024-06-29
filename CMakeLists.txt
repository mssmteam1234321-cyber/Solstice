cmake_minimum_required(VERSION 3.28)

include(cmake/CPM.cmake)

project(Solstice)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.9.2
)

CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        VERSION 0.9.5
)

CPMAddPackage(
        NAME nuvola_event_system
        GITHUB_REPOSITORY DisabledMallis/NuvolaEventSystem
        VERSION 1.1.0
)

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 1.0.1
)

CPMAddPackage(
        NAME b_embed
        GITHUB_REPOSITORY batterycenter/embed
        GIT_TAG v1.2.16
)

set(CMAKE_CXX_STANDARD 26)

add_compile_definitions(NOMINMAX)
add_compile_definitions(JM_XORSTR_DISABLE_AVX_INTRINSICS)

add_compile_definitions(ENTT_SPARSE_PAGE=2048)
add_compile_definitions(ENTT_PACKED_PAGE=128)

set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

if (${IS_DEBUG_BUILD})
    add_compile_definitions(__DEBUG__)
endif ()

# Debug options
add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
add_compile_options($<$<CONFIG:Debug>:/Zi>)

# Release options
add_compile_options($<$<CONFIG:Release>:/O2>) # optimize for speed
add_compile_options($<$<CONFIG:Release>:/Oi>) # enable intrinsic functions
add_compile_options($<$<CONFIG:Release>:/GL>) # enable link-time code generation
add_compile_options($<$<CONFIG:Release>:/Gy>) # separate functions for linker
add_compile_options($<$<CONFIG:Release>:/Gw>) # optimize global data
add_compile_options($<$<CONFIG:Release>:/GS>) # buffer security check
add_compile_options($<$<CONFIG:Release>:/Gm->) # disable minimal rebuild
add_compile_options($<$<CONFIG:Release>:/GF>) # enable string pooling
add_compile_options($<$<CONFIG:Release>:/fp:fast>) # optimize floating point
add_compile_options($<$<CONFIG:Release>:/GR->) # disable RTTI
add_compile_options($<$<CONFIG:Release>:/Gd>) # use __cdecl for all functions
add_compile_options($<$<CONFIG:Release>:/MT>) # use static multithreaded runtime
add_compile_options($<$<CONFIG:Release>:/Ob2>) # inline any suitable function
add_compile_options($<$<CONFIG:Release>:/Ot>) # favor fast code
add_compile_options($<$<CONFIG:Release>:/Oy->) # omit frame pointers




#add_compile_options($<$<CONFIG:Release>:/O2;/Oi;/GL;/Gy;/Gw;/GS;/Gm-;/GF;/fp:fast;/GR-;/Gd;/MT;/Ob2>)

set(SOLSTICE_VERSION "v1.0.0")
add_compile_definitions(SOLSTICE_VERSION="${SOLSTICE_VERSION}")

add_definitions(-DWIN32_LEAN_AND_MEAN)

include_directories(.)
include_directories(src)
include_directories(include)
include_directories(include/minhook/include)
include_directories(include/libhat)
include_directories(include/entt)
include_directories(include/libhat/libhat)
include_directories(include/ImGui)
include_directories(include/Kiero)

file(GLOB_RECURSE sourceFiles "src/*.cpp")

add_library(Solstice SHARED ${sourceFiles})

# get a list of all files in the resources directory (with relative paths)
file(GLOB_RECURSE resource_files_list "resources/*")
# convert to relative paths
set(resource_files "")
foreach(file ${resource_files_list})
    file(RELATIVE_PATH relative_file ${CMAKE_CURRENT_SOURCE_DIR}/resources ${file})
    list(APPEND resource_files resources/${relative_file})
    message(STATUS "Embedding resource: ${relative_file}")
    b_embed(Solstice resources/${relative_file})
endforeach()




add_subdirectory(include/minhook)
add_subdirectory(include/libhat)
add_subdirectory(include/ImGui)
add_subdirectory(include/Kiero)

target_include_directories(Solstice PRIVATE include/ImGui)
target_include_directories(Solstice PRIVATE include/Kiero)

target_link_libraries(Solstice PRIVATE spdlog glm minhook kiero nuvola_event_system libhat dbghelp.lib winmm.lib magic_enum d3d12.lib d3d11.lib d2d1.lib dxgi.lib dxguid.lib dwrite.lib winmm.lib)

set_property(TARGET Solstice minhook spdlog glm kiero nuvola_event_system magic_enum libhat PROPERTY MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDLL>$<$<CONFIG:Release>:MultiThreaded>")



