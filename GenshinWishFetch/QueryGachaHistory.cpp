
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

#include "GenshinWishFetch.h"
#include "GachaDataObjects.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/memorystream.h"
#include "fmt/core.h"
#include "fmt/color.h"

using std::list;
using std::string;
using std::map;

const char* GachaHistoryApi =				// Format string
"/event/gacha_info/api/getGachaLog?"
"win_mode=fullscreen&"
"authkey_ver=1&"
"sign_type=2&"
"auth_appid=webview_gacha&"
"device_type=pc&"
"plat_type=pc&"
"init_type={}&"									// %d	init_type
"gacha_id={}&"									// %s	gacha_id
"timestamp={}&"									// %d	timestamp
"lang={}&"										// %s	lang
"game_version={}&"								// %s	game_version
"region={}&"									// %s	region
"authkey={}&"									// %s	auth_key
"game_biz={}&"									// %s	game_biz
"gacha_type={}&"								// %d	gacha_type
"size={}&"										// %d	size
"end_id={}";									// %s	end_id

constexpr char *ServerDomainCN = "hk4e-api.mihoyo.com",
			   *ServerDomainOS = "hk4e-api-os.hoyoverse.com";

constexpr int ItemPerPage = 10;

std::list<SinglePullResult> GachaQueryResponseToPullResults(std::string jsonResp) {
	list<SinglePullResult> ret;

	rapidjson::Document doc;
	doc.Parse(jsonResp.c_str());

	int code = doc["retcode"].GetInt();
	if (code) {
		throw fmt::format("Query returned error Code = {} Message = {}", code, doc["message"].GetString());
	}

	auto data = doc["data"].GetObject();
	auto list = data["list"].GetArray();
	for (auto& i : list) {
		SinglePullResult res;
		res.Name = i["name"].GetString();
		res.PullId = std::stoull(i["id"].GetString());
		res.Rank = static_cast<ItemRank>(std::stoi(i["rank_type"].GetString()));
		std::stringstream timess(i["time"].GetString());
		timess >> std::get_time(&res.TimePulled, "%Y-%m-%d %H:%M:%S");
		res.Type = TypeStringToEnum(i["item_type"].GetString());

		ret.emplace_back(res);
	}

	return ret;
}

// FIXME: handle UID=0
unsigned int GachaQueryResultsToUid(std::string jsonResp) {
	unsigned int uid = 0;

	rapidjson::Document doc;
	doc.Parse(jsonResp.c_str());

	auto data = doc["data"].GetObject();
	auto list = data["list"].GetArray();
	if (list.Size() == 0)
		return uid;
	uid = std::stoul(list[0]["uid"].GetString());

	return uid;
}

//FIXME: GachaHistoryResponse
GachaHistoryResponse FetchGachaHistory(RequestGachaHistorySpec spec) {
	GachaHistoryResponse ret;

	// Set up headers
	map<string, string> headers{
		{"Cookie", spec.Language},
		{"Origin", "https://webstatic.mihoyo.com"},
		{"Connection", "keep-alive"},
		{"Referer", "https://webstatic.mihoyo.com/"}
	};
	//SetHeaders(headers);

	std::string serverDomain = ((spec.Region.find("cn") != string::npos) ? ServerDomainCN : ServerDomainOS);
	std::string reqResult; // Preserve reqResult, we find out UID after the loop
	unsigned long long endId = 0, // endId is required by query API to determine the begin position
		lastEndId = 0; // lastEndId is used to check if the query has reached the end
	int fetchedCount = 0;

	for (int page = 1; ; page++) {
		int errCode = 0;
		std::string queryObject = fmt::format(GachaHistoryApi,
											  (int)spec.Type,
											  spec.GachaId,
											  std::time(0),
											  spec.Language,
											  spec.GameVersion,
											  spec.Region,
											  spec.AuthKey,
											  spec.GameBiz,
											  (int)spec.Type,
											  ItemPerPage,
											  endId);

		reqResult = DoRequest(serverDomain, queryObject, true, errCode);

		if (reqResult.empty()) {
			fmt::print(fg(fmt::color::red), "Request {} failed with code {}\n", queryObject, errCode);
			exit(-1);
		}

		auto pullResults = GachaQueryResponseToPullResults(reqResult);

		// If the returned list is empty, break.
		if (pullResults.size() == 0)
			break;

		endId = pullResults.back().PullId;

		// Check if reached end by checking lastEndId
		// Because query API doesn't seem to provide total count information, we had to check by other means
		if (endId == lastEndId)
			break;

		// Then check if reached the specified pull id
		// If reached, delete the rest of results, append and break
		for (auto it = pullResults.begin(); it != pullResults.end(); it++) {
			if (it->PullId == spec.DownloadUntilGachaId) {
				pullResults.erase(it, pullResults.end());
				ret.Pulls.insert(ret.Pulls.end(), pullResults.begin(), pullResults.end());
				goto BreakMainFetchLoop;
			}
		}

		// If not reached end, append the results. Prevent duplicated results
		ret.Pulls.insert(ret.Pulls.end(), pullResults.begin(), pullResults.end());

		// Check if reached end by checking returned size
		// This one is checked after appending results because it doesn't cause duplicating
		if (pullResults.size() < ItemPerPage)
			break;

		// Break the loop for single shots
		if (spec.SingleShot)
			break;

		lastEndId = ret.Pulls.back().PullId;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

BreakMainFetchLoop:

	// Get UID
	ret.Uid = GachaQueryResultsToUid(reqResult);

	// Reverse the list
	ret.Pulls.reverse();

	return ret;
}
