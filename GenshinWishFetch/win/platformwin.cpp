
#include <string>
#include <map>
#include <assert.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinInet.h>

void TestHttpRequest() {
	auto hInternet
		= InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:107.0) Gecko/20100101 Firefox/107.0",
						INTERNET_OPEN_TYPE_DIRECT,
						NULL,
						NULL,
						NULL);
	assert(hInternet);

	auto hSession = InternetConnectA(hInternet, "hk4e-api.mihoyo.com", 443, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	assert(hSession);

	LPCSTR acceptedTypes[] = {"text/*", "application/*", "*/*", NULL};
	auto hRequest = HttpOpenRequestA(hSession, "GET", "/", NULL, NULL, acceptedTypes, INTERNET_FLAG_SECURE, NULL);
	assert(hRequest);

	BOOL bRequestResult = HttpSendRequestA(hRequest, "", 0, NULL, 0);
	if (!bRequestResult) {
		printf("%d", GetLastError());
		return;
	}

	CHAR readBuffer[512] { 0 };
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

// Private globals
static std::string RequestHeaders{},
				   ResponseHeaders{};

// Platform Implementations

void SetHeaders(std::map<std::string, std::string> headers) {
	RequestHeaders.clear();
	for (auto& [key, value] : headers) {
		RequestHeaders += key + ':' + value + "\r\n";
	}
}

std::string DoRequest(std::string domain, std::string object, bool https, int& errCode) {
	std::string ret{};

	auto hInternet
		= InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:107.0) Gecko/20100101 Firefox/107.0 GenshinWishFetch-RigoLigo/1.0",
						INTERNET_OPEN_TYPE_DIRECT,
						NULL,
						NULL,
						NULL);
	assert(hInternet);

	auto hSession = InternetConnectA(hInternet, domain.c_str(), (https ? 443 : 80), NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	errCode = GetLastError();
	if (!hSession) return ret;

	LPCSTR acceptedTypes[] = { "text/*", "application/*", "*/*", NULL };
	auto hRequest = HttpOpenRequestA(hSession, "GET", object.c_str(), NULL, NULL, acceptedTypes, (https ? INTERNET_FLAG_SECURE : NULL), NULL);
	errCode = GetLastError();
	if (!hRequest) return ret;

	BOOL bRequestResult = HttpSendRequestA(hRequest, RequestHeaders.c_str(), RequestHeaders.size(), NULL, 0);
	errCode = GetLastError();
	if (!bRequestResult) return ret;

	CHAR readBuffer[512]{ 0 };
	DWORD dwBytesRead;

	while (true) {
		BOOL bReadResult = InternetReadFile(hRequest, readBuffer, 512, &dwBytesRead);
		if (!bReadResult || dwBytesRead == 0)
			break;

		ret.append(readBuffer, dwBytesRead);
	}

	// Retrieve response headers.
	DWORD dwResponseHeaderSize;
	HttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &dwResponseHeaderSize, NULL);
	ResponseHeaders.clear();
	if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) { // If there's other errors, give up.
		ResponseHeaders.reserve(dwResponseHeaderSize);
		HttpQueryInfoA(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, ResponseHeaders.data(), &dwResponseHeaderSize, NULL);
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);

	return ret;
}

std::map<std::string, std::string> GetResponseHeaders() {
	std::map<std::string, std::string> ret;
	return ret;
}

void PlatformWarmup() {
	SetConsoleOutputCP(CP_UTF8);
}

void MakeDirectory(std::string dirname) {
	CreateDirectoryA(dirname.c_str(), NULL);
}
