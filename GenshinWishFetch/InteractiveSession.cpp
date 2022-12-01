
#include "GenshinWishFetch.h"
#include "GachaDataObjects.h"

using std::cout;
using std::cin;

void InteractiveSession() {
	std::string GachaUrl;
	
	cout << " >>> Genshin Impact Wish Data Fetcher <<<\n\n"
		"Paste Gacha URL here (https://webstatic.mihoyo.com/...) >>> ";

	std::getline(cin, GachaUrl);

	auto params = FormParametersFromGachaLink(GachaUrl);

	cout << "\n ==== Gacha URL Parameters ====\n\n";
	for (auto& [key, value] : params) {
		cout << key << " ==> " << value << '\n';
	}

	RequestGachaHistorySpec reqSpec;
	reqSpec.AuthKey = params["authkey"];
	reqSpec.GachaId = params["gacha_id"];
	reqSpec.GameBiz = params["game_biz"];
	reqSpec.GameVersion = params["game_version"];
	reqSpec.Language = params["lang"];
	reqSpec.Region = params["region"];
	reqSpec.Type = static_cast<GachaType>(std::stoi(params["gacha_type"]));


}
