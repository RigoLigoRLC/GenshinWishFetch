
#include <fstream>
#include <iomanip>
#include "GachaDataObjects.h"
#include "GenshinWishFetch.h"

using std::string;

bool GetPlayerMetadata(unsigned int uid, PlayerMetadata& metadata) {
	std::ifstream file(std::to_string(uid) + "/metadata", std::ios_base::in);
	if (!file) return false;
	
	string line;

	while(file) {
		std::getline(file, line);

		if (line == "LastLoggedGachaId") {
			int count, gachaType;
			unsigned long long gachaId;
			file >> count;
			for (int i = 0; i < count; i++) {
				file >> gachaType >> gachaId;
				metadata.LastLoggedGachaId[gachaType] = gachaId;
				//file.get();
			}
		}
		else if (line == "DoFetchNoviceWish") {
			file >> metadata.DoFetchNoviceWish;
		}
		else if (line == "End") {
			return true;
		}
		else {

		}
	}
}

bool WritePlayerMetadata(unsigned int uid, PlayerMetadata& metadata) {
	std::ofstream file(std::to_string(uid) + "/metadata", std::ios_base::out);
	if (!file) return false;

	file << "LastLoggedGachaId\n";
	file << metadata.LastLoggedGachaId.size() << '\n';
	for (auto& [k, v] : metadata.LastLoggedGachaId) {
		file << k << ' ' << v << '\n';
	}

	file << "DoFetchNoviceWish\n" << metadata.DoFetchNoviceWish << '\n';

	file << "End\n";

	file.close();
}

bool InitPlayerMetadata(unsigned int uid) {
	MakeDirectory(std::to_string(uid));

	PlayerMetadata metadata;
	metadata.LastLoggedGachaId = {
		{ 100, -1 },
		{ 200, -1 },
		{ 301, -1 },
		{ 302, -1 }
	};
	metadata.DoFetchNoviceWish = false;

	return WritePlayerMetadata(uid, metadata);
}

bool InitPlayerGachaHistoryFile(unsigned int uid, int type) {
	std::ofstream file(std::to_string(uid) + '/' + std::to_string(type) + ".csv",
					   std::ios_base::out);
	if (!file) return false;

	file << "_____Pull ID_____,__Item Name__,_Type_,Rank,__Timestamp__,______Time______,\n";
	file.close();

	return true;
}

bool CheckPlayerGachaHistoryFiles(unsigned int uid) {
	bool ret = true;

	for (auto i : { 100, 200, 301, 302 }) {
		std::fstream file(std::to_string(uid) + '/' + std::to_string(i) + ".csv");
		if (!file)
			ret &= InitPlayerGachaHistoryFile(uid, i);
		else
			file.close();
	}

	return ret;
}

bool AppendPlayerGachaHistoryFile(unsigned int uid, GachaType type, std::list<SinglePullResult> pulls) {
	std::ofstream file(std::to_string(uid) + '/' + std::to_string(type) + ".csv",
					   std::ios_base::out | std::ios_base::app);
	if (!file) return false;

	for (auto& i : pulls) {
		file << i.PullId << ','
			<< i.Name << ','
			<< i.Type << ','
			<< i.Rank << ','
			<< std::mktime(&i.TimePulled) << ','
			<< std::put_time(&i.TimePulled, "%Y-%m-%d %H:%M:%S") << ','
			<< ",\n";
	}

	file.close();
	return true;
}
