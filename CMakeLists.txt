cmake_minimum_required(VERSION 3.14)
project(rvi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(DEBUG_COMMON_FLAGS
    -D_DEBUG
    -ggdb3
    -std=c++20
    -O0
    -Wall
    -Wextra
    -Wpedantic
    -Weffc++
    -Wmissing-declarations
    -Wcast-align
    -Wcast-qual
    -Wchar-subscripts
    -Wconversion
    -Wctor-dtor-privacy
    -Wempty-body
    -Wfloat-equal
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-signedness
    -Wformat=2
    -Winline
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wpacked
    -Wpointer-arith
    -Winit-self
    -Wredundant-decls
    -Wshadow
    -Wextra-semi
    -Wsign-conversion
    -Wsign-promo
    -Wstrict-overflow=2
    -Wsuggest-override
    -Wswitch-default
    -Wswitch-enum
    -Wundef
    -Wunreachable-code
    -Wunused
    -Wvariadic-macros
    -Wno-missing-field-initializers
    -Wfloat-conversion
    -Wdouble-promotion
    -Wno-narrowing
    -Wno-old-style-cast
    -Wno-varargs
    -Wstack-protector
    -fcheck-new
    -fsized-deallocation
    -fstack-protector
    -fstrict-overflow
    -fno-omit-frame-pointer
    -fPIE
    -Werror=vla
    -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

# Loguru
set(LOGURU_WITH_STREAMS TRUE)
add_subdirectory(external/loguru)

include(CTest)

# ---- Main ----
add_executable(rvi
  ${PROJECT_SOURCE_DIR}/source/main.cpp
  ${PROJECT_SOURCE_DIR}/include/rv32i/rvi_rv32i_registration.cpp
  ${PROJECT_SOURCE_DIR}/include/rv32m/rvi_rv32m_registration.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_mmap_file.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_decode_info.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_instruction_interface.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_instruction_registry.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_memory_state.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_parse_elf.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_read_binary.cpp
  ${PROJECT_SOURCE_DIR}/source/rvi_state.cpp
)
target_link_libraries(rvi PRIVATE loguru)
target_include_directories(rvi PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(rvi PRIVATE
  ${PROJECT_SOURCE_DIR}/external/loguru
)
target_include_directories(rvi PRIVATE
  ${PROJECT_SOURCE_DIR}/external/cxxopts/include
)

target_compile_options(rvi PRIVATE
    $<$<CONFIG:Debug>:${DEBUG_COMMON_FLAGS}>
)

target_link_options(rvi PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -fstack-protector -fPIE -pie>
)

# ---- Tests ----
if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(googletest)

  add_executable(rviTests
    ${PROJECT_SOURCE_DIR}/source/test.cpp
  )
  target_link_libraries(rviTests PRIVATE loguru GTest::gtest_main)
  target_include_directories(rviTests PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  )
  target_include_directories(rviTests PRIVATE
    ${PROJECT_SOURCE_DIR}/external/loguru
  )

  if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()

  include(GoogleTest)
  gtest_discover_tests(rviTests)
endif()
