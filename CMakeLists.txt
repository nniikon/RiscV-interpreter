cmake_minimum_required(VERSION 3.14)
project(rvi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Loguru
set(LOGURU_WITH_STREAMS TRUE)
add_subdirectory(external/loguru)

include(CTest)

# ---- Main ----
add_executable(rvi
  ${PROJECT_SOURCE_DIR}/source/main.cpp
)
target_link_libraries(rvi PRIVATE loguru)
target_include_directories(rvi PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(rvi PRIVATE
  ${PROJECT_SOURCE_DIR}/external/loguru
)

# ---- Tests ----
if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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
