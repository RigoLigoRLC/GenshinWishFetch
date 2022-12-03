
#include "GenshinWishFetch.h"
#include "GachaDataObjects.h"
#include "fmt/core.h"
#include "fmt/color.h"

using std::cout;
using std::cin;

void InteractiveSession() {
	std::string GachaUrl;
	
	cout << " >>> Genshin Impact Wish Data Fetcher <<<\n\n"
		"Paste Gacha URL here (https://webstatic.mihoyo.com/...) >>> ";

	std::getline(cin, GachaUrl);

	auto params = ParametersFromGachaLink(GachaUrl);

	// Construct request spec
	RequestGachaHistorySpec reqSpec;
	reqSpec.AuthKey = params["authkey"];
	reqSpec.GachaId = params["gacha_id"];
	reqSpec.GameBiz = params["game_biz"];
	reqSpec.GameVersion = params["game_version"];
	reqSpec.Language = params["lang"];
	reqSpec.Region = params["region"];
	reqSpec.Type = CharacterEventWish;
	reqSpec.DownloadUntilGachaId = -1;

	SetRegion(reqSpec.Region);

	// Special case processing: game_biz may include a pound at the end which causes issues.
	if (reqSpec.GameBiz.back() == '#')
		reqSpec.GameBiz.pop_back();

	try {
		// Get UID from history
		reqSpec.SingleShot = true;
		auto historyForUid = FetchGachaHistory(reqSpec);
		auto uid = historyForUid.Uid;
		reqSpec.SingleShot = false;

		fmt::print(fg(fmt::color::light_green), "UID: {}\n", uid);

		// Get player metadata
		PlayerMetadata metadata;
		bool r = GetPlayerMetadata(uid, metadata);
		if (!r) {
			if (!InitPlayerMetadata(uid))
				fmt::print(fg(fmt::color::red) | bg(fmt::color::white), "Can't create metadata for {}!\n", uid);
			else {
				fmt::print(fg(fmt::color::cyan), "Created metadata for {}.\n", uid);
				GetPlayerMetadata(uid, metadata);
			}
		}
		if (!CheckPlayerGachaHistoryFiles(uid))
		   fmt::print(fg(fmt::color::red) | bg(fmt::color::white), "One or more data files aren't created!\n", uid);

		for (GachaType i : {NoviceWish, StandardWish, CharacterEventWish, WeaponEventWish}) {
			if (i == NoviceWish) {
				if(!metadata.DoFetchNoviceWish)
					continue;
				else {
					fmt::print(fg(fmt::color::yellow),
							   "You manually turned on Fetch Novice Wish. "
							   "The switch is turned off next time you fetch your gacha history.\n");
					metadata.DoFetchNoviceWish = false;
				}
			}

			reqSpec.Type = i;
			reqSpec.DownloadUntilGachaId = metadata.LastLoggedGachaId[i];

			auto history = FetchGachaHistory(reqSpec);
			AppendPlayerGachaHistoryFile(uid, i, history.Pulls);

			fmt::print(fg(fmt::color::green), "Added {} records to gacha type {}\n", history.Pulls.size(), i);
			
			if(history.Pulls.size())
				metadata.LastLoggedGachaId[i] = history.Pulls.back().PullId;
		}

		if (WritePlayerMetadata(uid, metadata))
			fmt::print(fg(fmt::color::cyan), "Refreshed player metadata.\n");
		else
			fmt::print(fg(fmt::color::red), "Cannot refresh player metadata!\n");
	}
	catch (std::string str) {
		fmt::print(fg(fmt::color::red) | bg(fmt::color::white), "{}\n", str);
	}
	catch (std::exception e) {
		fmt::print(fg(fmt::color::red) | bg(fmt::color::white), "Caught exception, what = {}\n", e.what());
	}
	catch (...) {
		fmt::print(fg(fmt::color::red) | bg(fmt::color::white), "Caught unknown stuff ¯\\_(ツ)_/¯\n");
	}
}
