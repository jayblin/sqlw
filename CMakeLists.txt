cmake_minimum_required(VERSION 3.23)

project(sqlw VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(SQLW_STANDALONE TRUE)
endif()

option(SQLW_BUILD_TESTS "Build test programs" ${SQLW_STANDALONE})

set(SQLW_EXEC_LIMIT 256 CACHE STRING "Default limit for consecutive queries and for SELECT results" FORCE)

add_library(
	sqlw
	SHARED
		vendor/sqlite/sqlite3.c
		src/connection.cpp
		src/statement.cpp
		src/status.cpp
		src/json_string_result.cpp
)

target_include_directories(
	sqlw
	PUBLIC include
	vendor/sqlite/include
)

configure_file(
	${PROJECT_SOURCE_DIR}/include/sqlw/cmake_vars.h.in
	${PROJECT_SOURCE_DIR}/include/sqlw/cmake_vars.h
)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")

set(
	GCC_COMPILER_FLAGS
	"-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused"
	CACHE
	STRING
	"Flags for gcc-like compiler"
)

target_compile_options(
	sqlw
	PUBLIC "$<${gcc_like_cxx}:${GCC_COMPILER_FLAGS}>"
)

include(FetchContent)

FetchContent_Declare(
	GSL
	GIT_REPOSITORY https://github.com/microsoft/GSL.git
	GIT_TAG v4.0.0
	GIT_SHALLOW ON
)

FetchContent_MakeAvailable(GSL)

target_link_libraries(
	sqlw
	PUBLIC Microsoft.GSL::GSL
)

# TESTS

# https://google.github.io/googletest/quickstart-cmake.html
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_custom_target(sqlw_tests)

add_executable(
	sqlw_tests_executable
	tests/status.cpp
	tests/connection.cpp
	tests/statement.cpp
	tests/json_string_result.cpp
)

add_dependencies(sqlw_tests sqlw_tests_executable)

target_link_libraries(
	sqlw_tests_executable
	PRIVATE GTest::gtest_main sqlw
)

include(GoogleTest)

gtest_discover_tests(sqlw_tests_executable)

# ~TESTS
