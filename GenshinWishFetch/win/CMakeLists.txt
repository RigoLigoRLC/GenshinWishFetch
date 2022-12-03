
add_library(GwfPlatformWin STATIC
			platformwin.cpp)

target_link_libraries(GwfPlatformWin wininet)
target_include_directories(GwfPlatformWin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GwfPlatformWin PROPERTY CXX_STANDARD 17)
endif()
