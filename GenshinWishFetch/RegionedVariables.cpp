
#include <string>
#include "GachaDataObjects.h"

static bool isRegionChina = false;

void SetRegion(std::string region) {
	isRegionChina = (region.find("cn") != std::string::npos);
}

// Dont ask me about other languages
// I dont fucking care

ItemType TypeStringToEnum(std::string type) {
	if (isRegionChina) {
		if (type == u8"ÎäÆ÷")
			return Weapon;
		else
			return Character;
	}
	else {
		if (type == "Weapon")
			return Weapon;
		else
			return Character;
	}
}
