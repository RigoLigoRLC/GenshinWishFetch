
cmake_minimum_required (VERSION 3.8)

# Platform Specific Libraries
if(WIN32)
	add_subdirectory("win")
	set(GWF_PLATFORM_LIBRARY GwfPlatformWin)
endif()

add_executable (GenshinWishFetch "GenshinWishFetch.cpp" "GenshinWishFetch.h" "UrlProcessing.cpp" "InteractiveSession.cpp" "GachaDataObjects.h" "GachaDataProcessing.cpp")
target_link_libraries(GenshinWishFetch ${GWF_PLATFORM_LIBRARY})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GenshinWishFetch PROPERTY CXX_STANDARD 17)
endif()

