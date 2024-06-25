cmake_minimum_required(VERSION 3.28)

include(cmake/CPM.cmake)

project(Solstice)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.9.2
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
include_directories(include/minhook/include)
include_directories(Includes/libhat)
include_directories(Includes/libhat/libhat)

file(GLOB_RECURSE sourceFiles "src/*.cpp")

function(add_resources out_var)
    set(result)
    foreach(in_f ${ARGN})
        file(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        set(out_f "${PROJECT_BINARY_DIR}/${in_f}.o")
        add_custom_command(OUTPUT ${out_f}
                COMMAND ld -r -b binary -o ${out_f} ${src_f}
                DEPENDS ${in_f}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Building object ${out_f}"
                VERBATIM
        )
        list(APPEND result ${out_f})
    endforeach()
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()

# (no resources to add for now)

add_library(Solstice SHARED ${sourceFiles} ${resourceFiles})

add_subdirectory(include/minhook)
add_subdirectory(include/libhat)

target_link_libraries(Solstice PRIVATE spdlog minhook libhat dbghelp.lib winmm.lib)

set_property(TARGET Solstice minhook spdlog libhat PROPERTY MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDLL>$<$<CONFIG:Release>:MultiThreaded>")



