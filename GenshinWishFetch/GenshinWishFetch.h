
#pragma once

#include <iostream>
#include <string>
#include <list>
#include <map>

#include "platform.h"
#include "GachaDataObjects.h"

// UrlProcessing.cpp
std::map<std::string, std::string> ParametersFromGachaLink(std::string GachaLink);

// InteractiveSession.cpp
void InteractiveSession();

// HttpRequests.cpp
GachaHistoryResponse FetchGachaHistory(RequestGachaHistorySpec spec);

// RegionedVariables.cpp
ItemType TypeStringToEnum(std::string type);
void SetRegion(std::string region);

// DataFile.cpp
bool GetPlayerMetadata(unsigned int uid, PlayerMetadata& metadata);
bool WritePlayerMetadata(unsigned int uid, PlayerMetadata& metadata);
bool InitPlayerMetadata(unsigned int uid);

bool InitPlayerGachaHistoryFile(unsigned int uid, int type);
bool CheckPlayerGachaHistoryFiles(unsigned int uid);
bool AppendPlayerGachaHistoryFile(unsigned int uid, GachaType type, std::list<SinglePullResult> pulls);
