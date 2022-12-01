
#pragma once

#include <iostream>
#include <string>
#include <map>

#include "platform.h"

// UrlProcessing.cpp
std::map<std::string, std::string> FormParametersFromGachaLink(std::string GachaLink);

// InteractiveSession.cpp
void InteractiveSession();