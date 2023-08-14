include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v4.1.1
)
set(FTXUI_BUILD_EXAMPLES OFF)
set(FTXUI_BUILD_DOCS OFF)
set(FTXUI_BUILD_TESTS OFF)
set(FTXUI_BUIL_TESTS_FUZZER OFF)
set(FTXUI_CLANG_TIDY OFF)
set(FTXUI_ENABLE_COVERAGE OFF)
set(FTXUI_DEV_WARNINGS OFF)
FetchContent_GetProperties(ftxui)
if (NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
