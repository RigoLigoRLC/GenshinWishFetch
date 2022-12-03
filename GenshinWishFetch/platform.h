
#pragma once

#include <string>
#include <map>

// HTTP Requests
void SetHeaders(std::map<std::string, std::string> headers);
std::string DoRequest(std::string domain, std::string object, bool https, int& errCode);
std::map<std::string, std::string> GetResponseHeaders();

//¡¡Misc
void PlatformWarmup();
void MakeDirectory(std::string dirname);