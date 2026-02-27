cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0077 NEW)

# CMake 4.x removed compat with < 3.5; zvec's thirdparty still uses old versions
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(zvec_ext LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Fetch Rice (header-only C++ Ruby bindings) ---
include(FetchContent)
FetchContent_Declare(
  rice
  GIT_REPOSITORY https://github.com/ruby-rice/rice.git
  GIT_TAG        4.11.2
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(rice)

# Rice's FindRuby.cmake properly detects rbenv Ruby
list(PREPEND CMAKE_MODULE_PATH "${rice_SOURCE_DIR}")
find_package(Ruby REQUIRED COMPONENTS Development)

# --- Build zvec as subdirectory ---
set(ZVEC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/zvec")

# Disable parts we don't need
set(BUILD_PYTHON_BINDINGS OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

# zvec uses PROJECT_ROOT_DIR internally
set(PROJECT_ROOT_DIR "${ZVEC_ROOT}" CACHE PATH "" FORCE)

# Forward CMAKE_PREFIX_PATH to Arrow's ExternalProject_Add via CMAKE_CACHE_ARGS
# (Arrow's CONFIGURE_COMMAND interpolates ${CMAKE_CACHE_ARGS} but it's unset by default)
if(CMAKE_PREFIX_PATH)
  set(CMAKE_CACHE_ARGS "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
endif()

# --- Fix antlr4 CMakeLists.txt for CMake 4.x compatibility ---
# zvec's own patch (antlr4.patch) runs first via apply_patch_once(), but it doesn't
# fix the OLD policies that CMake 4.x removed. We apply additional fixups here.
set(_antlr4_cmake "${ZVEC_ROOT}/thirdparty/antlr/antlr4/runtime/Cpp/CMakeLists.txt")
set(_antlr4_cmake4_mark "${ZVEC_ROOT}/thirdparty/antlr/antlr4/.cmake4_compat_patched")
if(NOT EXISTS "${_antlr4_cmake4_mark}" AND EXISTS "${_antlr4_cmake}")
  file(READ "${_antlr4_cmake}" _antlr4_content)
  # Fix cmake_minimum_required to 3.5
  string(REPLACE "cmake_minimum_required (VERSION 2.8)" "cmake_minimum_required (VERSION 3.5)" _antlr4_content "${_antlr4_content}")
  # Remove OLD policy settings (CMake 4.x removed these)
  string(REPLACE "  CMAKE_POLICY(SET CMP0054 OLD)\n  CMAKE_POLICY(SET CMP0045 OLD)\n  CMAKE_POLICY(SET CMP0042 OLD)\n" "" _antlr4_content "${_antlr4_content}")
  string(REPLACE "  CMAKE_POLICY(SET CMP0059 OLD)\n  CMAKE_POLICY(SET CMP0054 OLD)\n" "" _antlr4_content "${_antlr4_content}")
  # Fix old-style endif() with arguments
  string(REPLACE "endif(NOT CMAKE_BUILD_TYPE)" "endif()" _antlr4_content "${_antlr4_content}")
  string(REPLACE "endif(NOT WITH_DEMO)" "endif()" _antlr4_content "${_antlr4_content}")
  string(REPLACE "endif(WITH_DEMO)" "endif()" _antlr4_content "${_antlr4_content}")
  string(REPLACE "endif(WIN32)" "endif()" _antlr4_content "${_antlr4_content}")
  string(REPLACE "if(NOT ANTLR4_CMAKE_DIR)" "if(NOT DEFINED ANTLR4_CMAKE_DIR OR NOT ANTLR4_CMAKE_DIR)" _antlr4_content "${_antlr4_content}")
  string(REPLACE "endif(NOT ANTLR4_CMAKE_DIR)" "endif()" _antlr4_content "${_antlr4_content}")
  string(REPLACE "endif(ANTLR4_INSTALL)" "endif()" _antlr4_content "${_antlr4_content}")
  file(WRITE "${_antlr4_cmake}" "${_antlr4_content}")
  file(WRITE "${_antlr4_cmake4_mark}" "patched")
  message(STATUS "Applied CMake 4.x compatibility fixes to antlr4")
endif()

add_subdirectory("${ZVEC_ROOT}" "${CMAKE_CURRENT_BINARY_DIR}/zvec" EXCLUDE_FROM_ALL)

# --- Ruby extension module ---
add_library(zvec_ext MODULE
  zvec/zvec_ext.cpp
  zvec/zvec_types.cpp
  zvec/zvec_status.cpp
  zvec/zvec_params.cpp
  zvec/zvec_schema.cpp
  zvec/zvec_doc.cpp
  zvec/zvec_collection.cpp
  zvec/zvec_config.cpp
)

target_include_directories(zvec_ext PRIVATE
  "${rice_SOURCE_DIR}/include"
  "${ZVEC_ROOT}/src/include"
  "${ZVEC_ROOT}/src"
)

# Link Rice (header-only) and Ruby
target_link_libraries(zvec_ext PRIVATE Rice::Rice)
target_link_libraries(zvec_ext PRIVATE Ruby::Module)

# Algorithm libraries that need force-loading for self-registration
set(ZVEC_ALGO_LIBS
  core_knn_flat_static
  core_knn_flat_sparse_static
  core_knn_hnsw_static
  core_knn_hnsw_sparse_static
  core_knn_ivf_static
  core_knn_cluster_static
  core_mix_reducer_static
  core_metric_static
  core_utility_static
  core_quantizer_static
)

# Explicit dependency so EXCLUDE_FROM_ALL targets get built
add_dependencies(zvec_ext ${ZVEC_ALGO_LIBS} zvec_db)

# Force-link zvec algorithm libraries (macOS) — required for self-registering algorithms
foreach(lib ${ZVEC_ALGO_LIBS})
  target_link_libraries(zvec_ext PRIVATE -Wl,-force_load,$<TARGET_FILE:${lib}>)
endforeach()
target_link_libraries(zvec_ext PRIVATE zvec_db)

# Output directly to lib/ with correct Ruby suffix
set_target_properties(zvec_ext PROPERTIES
  PREFIX ""
  SUFFIX ".bundle"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
)

# Suppress -Werror=return-type from zvec propagating to Rice headers
target_compile_options(zvec_ext PRIVATE -Wno-error=return-type)
