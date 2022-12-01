
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinInet.h>
#include <assert.h>
#include <stdio.h>
#include <string>

void TestHttpRequest() {
	auto hInternet
		= InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:107.0) Gecko/20100101 Firefox/107.0",
						INTERNET_OPEN_TYPE_DIRECT,
						NULL,
						NULL,
						NULL);
	assert(hInternet);

	auto hSession = InternetConnectA(hInternet, "microsoft.com", 443, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	assert(hSession);

	LPCSTR acceptedTypes[] = {"text/*", NULL};
	auto hRequest = HttpOpenRequestA(hSession, "GET", "/", NULL, NULL, acceptedTypes, INTERNET_FLAG_SECURE, NULL);
	assert(hRequest);

	BOOL bRequestResult = HttpSendRequestA(hRequest, "", 0, NULL, 0);
	if (!bRequestResult) {
		printf("%d", GetLastError());
		return;
	}

	CHAR readBuffer[512];
	DWORD dwBytesRead;

	while (true) {
		BOOL bReadResult = InternetReadFile(hRequest, readBuffer, 512, &dwBytesRead);
		if (!bReadResult || dwBytesRead == 0)
			break;

		fwrite(readBuffer, 1, dwBytesRead, stdout);
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
}