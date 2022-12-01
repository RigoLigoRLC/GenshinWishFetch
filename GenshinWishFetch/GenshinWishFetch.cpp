
#include "GenshinWishFetch.h"

using namespace std;

int main(int argc, char** argv)
{
	//TestHttpRequest();
	auto params = FormParametersFromGachaLink("https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170");
	for (auto pair : params) {
		cout << pair.first << " ==> " << pair.second << '\n';
	}

	return 0;
}
