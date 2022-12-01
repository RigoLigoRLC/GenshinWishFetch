
add_library(GwfPlatformWin STATIC
			platformwin.cpp)

target_link_libraries(GwfPlatformWin wininet)
target_include_directories(GwfPlatformWin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
