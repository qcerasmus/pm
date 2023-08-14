file(GLOB pm_sources *.cpp)
file(GLOB pm_headers *.h)

add_executable(pm ${pm_headers} ${pm_sources})
target_link_libraries(pm
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE clip
)
target_include_directories(pm PUBLIC ${clip_SOURCE_DIR})
