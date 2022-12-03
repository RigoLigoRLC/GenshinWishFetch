
#pragma once

#include <string>
#include <list>
#include <map>

enum ItemRank {
	Rank3Star			= 3,
	Rank4Star			= 4,
	Rank5Star			= 5,
};

enum ItemType {
	Weapon,
	Character,
};

enum GachaType {
	NoviceWish			= 100,
	StandardWish		= 200,
	CharacterEventWish	= 301,
	WeaponEventWish		= 302,
};

struct SinglePullResult {
	std::string			Name;
	ItemRank			Rank;
	ItemType			Type;
	std::tm				TimePulled;
	unsigned long long  PullId;
};

struct RequestGachaHistorySpec {
	// Required fields for making request
	std::string			Region;
	std::string			AuthKey;
	std::string			GameVersion;
	std::string			Language;
	std::string			GameBiz;
	std::string			GachaId;
	GachaType			Type;

	// Run the fetch loop only once. Used to retrieve UID.
	bool SingleShot;

	// Download gacha records until this pull.
	// The specified pull is not included in returned data.
	// If Id = -1, the entirety of gacha history will be downloaded, and under this situation,
	// the default behavior should be attempting to merge the downloaded data with
	// data on the disk.
	// Otherwise, the downloaded data is directly appended to data on disk without checks.
	unsigned long long  DownloadUntilGachaId;
};

struct GachaHistoryResponse {
	// Because only after downloading gacha history can we determine the UID, the UID will
	// only be returned in response, rather than being determined before downloading.
	// The returned pulls data is stored in a linked list, with incrementing PullID. This should
	// imply an increasing TimePulled.

	uint32_t						Uid;
	std::list<SinglePullResult>		Pulls;
};

struct PlayerMetadata {
	// The gacha ID of last pull retrieved last time. Key = Gacha Type, Val = Gacha ID
	std::map<int, unsigned long long> LastLoggedGachaId;

	// Novice wish. Defaults to off. Turned off after using one time.
	bool DoFetchNoviceWish;
};
