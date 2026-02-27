cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0077 NEW)

# CMake 4.x removed compat with < 3.5; zvec's thirdparty still uses old versions
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

project(zvec_ext LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ICU4C is required by Arrow (via Boost.Locale). On macOS with Homebrew,
# add the default path if CMAKE_PREFIX_PATH isn't already set (presets set it).
if(APPLE AND NOT CMAKE_PREFIX_PATH)
  set(_ICU_PATH "/opt/homebrew/opt/icu4c@78")
  if(EXISTS "${_ICU_PATH}")
    list(APPEND CMAKE_PREFIX_PATH "${_ICU_PATH}")
    message(STATUS "Added ICU4C to CMAKE_PREFIX_PATH: ${_ICU_PATH}")
  endif()
endif()

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

# ============================================================================
# Three-tier zvec resolution:
#   1. Pre-installed library (e.g., brew install zvec) — fast path
#   2. Local git submodule at vendor/zvec — development path
#   3. FetchContent from GitHub — fallback for gem install without Homebrew
# ============================================================================

# --- Strategy 1: Find pre-installed zvec (e.g., from Homebrew) ---
find_library(ZVEC_LIBRARY zvec
  HINTS /opt/homebrew/opt/zvec/lib
        /usr/local/opt/zvec/lib
        ENV ZVEC_DIR
)
find_path(ZVEC_INCLUDE_DIR zvec/db/collection.h
  HINTS /opt/homebrew/opt/zvec/include
        /usr/local/opt/zvec/include
        ENV ZVEC_DIR
)

if(ZVEC_LIBRARY AND ZVEC_INCLUDE_DIR)
  message(STATUS "Found pre-installed zvec: ${ZVEC_LIBRARY}")
  message(STATUS "  includes: ${ZVEC_INCLUDE_DIR}")
  set(ZVEC_PREBUILT TRUE)
else()
  set(ZVEC_PREBUILT FALSE)

  # Disable parts of zvec we don't need (must be set before add_subdirectory)
  set(BUILD_PYTHON_BINDINGS OFF CACHE BOOL "" FORCE)
  set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

  # --- Strategy 2: Local submodule (development) ---
  set(_ZVEC_LOCAL "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/zvec")

  if(EXISTS "${_ZVEC_LOCAL}/CMakeLists.txt")
    message(STATUS "Using local zvec from ${_ZVEC_LOCAL}")
    set(ZVEC_ROOT "${_ZVEC_LOCAL}")
  else()
    # --- Strategy 3: FetchContent from GitHub ---
    message(STATUS "vendor/zvec not found — fetching from GitHub")
    FetchContent_Declare(
      zvec_src
      GIT_REPOSITORY https://github.com/alibaba/zvec.git
      GIT_TAG        1d5aeeed1df6e104443edf23891798f1ff0a8bf7
      GIT_SHALLOW    FALSE
      GIT_SUBMODULES_RECURSE TRUE
    )
    FetchContent_GetProperties(zvec_src)
    if(NOT zvec_src_POPULATED)
      FetchContent_Populate(zvec_src)
    endif()
    set(ZVEC_ROOT "${zvec_src_SOURCE_DIR}")

    # zvec's subdirectory CMakeLists expect cmake/ relative to CMAKE_SOURCE_DIR
    # (in development, ext/cmake is a symlink to ../vendor/zvec/cmake)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/cmake/bazel.cmake")
      file(COPY "${ZVEC_ROOT}/cmake/" DESTINATION "${CMAKE_SOURCE_DIR}/cmake/")
    endif()
  endif()

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
endif()

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

# Link Rice (header-only) and Ruby
target_link_libraries(zvec_ext PRIVATE Rice::Rice)
target_link_libraries(zvec_ext PRIVATE Ruby::Module)

if(ZVEC_PREBUILT)
  # --- Fast path: link against pre-installed fat static archive ---
  target_include_directories(zvec_ext PRIVATE
    "${rice_SOURCE_DIR}/include"
    "${ZVEC_INCLUDE_DIR}"
  )
  target_link_libraries(zvec_ext PRIVATE -Wl,-force_load,${ZVEC_LIBRARY})

  # ICU4C is required at runtime (Arrow uses it for Unicode support)
  find_package(ICU COMPONENTS uc data i18n)
  if(ICU_FOUND)
    target_link_libraries(zvec_ext PRIVATE ICU::uc ICU::data ICU::i18n)
  endif()
else()
  # --- Source build: link against individual zvec targets ---
  target_include_directories(zvec_ext PRIVATE
    "${rice_SOURCE_DIR}/include"
    "${ZVEC_ROOT}/src/include"
    "${ZVEC_ROOT}/src"
  )

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
endif()

# Output directory: use CMAKE_LIBRARY_OUTPUT_DIRECTORY if set by RubyGems,
# otherwise default to the local lib/ directory (development builds)
if(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(_EXT_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
else()
  set(_EXT_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
endif()

set_target_properties(zvec_ext PROPERTIES
  PREFIX ""
  SUFFIX ".bundle"
  LIBRARY_OUTPUT_DIRECTORY "${_EXT_OUTPUT_DIR}"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${_EXT_OUTPUT_DIR}"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${_EXT_OUTPUT_DIR}"
)

# Suppress -Werror=return-type from zvec propagating to Rice headers
target_compile_options(zvec_ext PRIVATE -Wno-error=return-type)
