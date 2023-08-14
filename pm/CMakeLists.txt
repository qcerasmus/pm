file(GLOB pm_sources *.cpp ${xxtea_SOURCE_DIR}/example/base64.c)
file(GLOB pm_headers *.h)

add_executable(pm ${pm_headers} ${pm_sources})
target_link_libraries(pm
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE clip
  PRIVATE xxtea
)
target_include_directories(pm PUBLIC ${clip_SOURCE_DIR} ${xxtea_SOURCE_DIR})
