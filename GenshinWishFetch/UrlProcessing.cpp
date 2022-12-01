
#include <string>
#include <map>
#include <string_view>

using std::string;
using std::map;
using std::pair;

std::map<std::string, std::string> FormParametersFromGachaLink(string GachaLink) {
	map<string, string> ret;

	// Capture the part between '?' and the next '/'. See example:
	// https://webstatic.mihoyo.com/hk4e/event/e20190909gacha-v2/index.html?[...hk4e_cn#]/log
	auto idxQuestionMark = GachaLink.find_first_of('?');
	if (idxQuestionMark == string::npos)
		return ret;

	auto idxNextSlash = GachaLink.find_first_of('/', idxQuestionMark);
	if (idxQuestionMark == idxNextSlash - 1) // params length is zero
		return ret;

	string params = GachaLink.substr(idxQuestionMark + 1, idxNextSlash - idxQuestionMark - 1);

	// Split each parameter
	size_t from = 0, to;
	do {
		to = params.find('&', from);

		// Get each K-V pair (in "K=V" form)
		// If we reached end of `params`, use (params.size()) instead of search result as the end pos
		std::string_view kvPair(params.c_str() + from, (to == string::npos ? params.size() : to) - from);

		// Find equal sign and split from there
		auto equalSignPos = kvPair.find('=');

		if (equalSignPos + 1 >= kvPair.size())
			continue;

		// Add K-V pair
		ret[string(kvPair.substr(0, equalSignPos))] = string(kvPair.substr(equalSignPos + 1));

		// Go to next param
		from = to + 1;
	} while (to != string::npos);
	
	return ret;
}
