
enable_testing()

if(NOT DEFINED CACHE{BUILD_UNICODE_TESTS})
  set(BUILD_UNICODE_TESTS ON CACHE BOOL "whether to build unicode tests")
endif()

if(BUILD_UNICODE_TESTS)

  add_executable(TEST_unicode main.cpp)
  target_link_libraries(TEST_unicode unicode-header-only)

  add_test(NAME TEST_unicode COMMAND TEST_unicode)

endif()
