cmake_minimum_required(VERSION 3.20)
project(typescript_script VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SCRIPT_SOURCES CONFIGURE_DEPENDS assets/scripts/*.ts)

include(FetchContent)

set(WASM3_ENABLE_WASI OFF CACHE BOOL "Disable WASI support")
set(WASM3_ENABLE_LIBC OFF CACHE BOOL "Disable libc support")

FetchContent_Declare(
  wasm3
  GIT_REPOSITORY https://github.com/wasm3/wasm3.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(wasm3)

add_executable(wasm_system
  src/main.cpp)

target_include_directories(wasm_system PRIVATE
  ${wasm3_SOURCE_DIR}/source
  src
)

target_link_libraries(wasm_system m3)

set(WASM_OUTPUTS "")
foreach(SCRIPT ${SCRIPT_SOURCES})
  get_filename_component(SCRIPT_NAME ${SCRIPT} NAME_WE)

  set(TEMP_SCRIPT ${CMAKE_BINARY_DIR}/temp/${SCRIPT_NAME}_build.ts)
  set(WASM_OUTPUT ${CMAKE_BINARY_DIR}/assets/wasm/${SCRIPT_NAME}.wasm)

  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets/wasm)

  add_custom_command(
    OUTPUT ${WASM_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "catting prelude and ${SCRIPT} into ${TEMP_SCRIPT}"
    COMMAND cat ${CMAKE_SOURCE_DIR}/assets/prelude.ts ${SCRIPT} > ${TEMP_SCRIPT}
    COMMAND ${CMAKE_COMMAND} -E echo "compiling ${TEMP_SCRIPT} to ${WASM_OUTPUT}"
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR} asc ${TEMP_SCRIPT} --target release --outFile ${WASM_OUTPUT} --runtime minimal --noAssert
    DEPENDS ${SCRIPT} assets/prelude.ts
    COMMENT "Building ${WASM_OUTPUT}"
  )

  list(APPEND WASM_OUTPUTS ${WASM_OUTPUT})
endforeach()

add_custom_target(build_wasm ALL DEPENDS ${WASM_OUTPUTS})
add_dependencies(wasm_system build_wasm)