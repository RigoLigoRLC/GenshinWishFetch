
cmake_minimum_required (VERSION 3.8)

# Platform Specific Libraries
if(WIN32)
	add_subdirectory("win")
	set(GWF_PLATFORM_LIBRARY GwfPlatformWin)
endif()

# Add RapidJson files recursively
file(GLOB_RECURSE RAPIDJSON_INCLUDES rapidjson/*.h)

# Add Fmt files
add_subdirectory(fmt)

add_executable (GenshinWishFetch
	${RAPIDJSON_INCLUDES}
	${FMT_INCLUDES}
	"GenshinWishFetch.cpp"
	"GenshinWishFetch.h"
	"UrlProcessing.cpp"
	"InteractiveSession.cpp"
	"GachaDataObjects.h"
	"GachaDataProcessing.cpp"
	"QueryGachaHistory.cpp"
	"platform.h"
	"RegionedVariables.cpp"
	"DataFile.cpp")
target_link_libraries(GenshinWishFetch ${GWF_PLATFORM_LIBRARY} fmt::fmt-header-only)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GenshinWishFetch PROPERTY CXX_STANDARD 17)
endif()

