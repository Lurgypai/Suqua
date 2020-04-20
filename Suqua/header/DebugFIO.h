#pragma once
#include <string>
#include <unordered_map>
#include <fstream>

class DebugFIO {
public:
	static void AddFOut(const std::string& file);
	static std::ostream& Out(const std::string& file);
	static void RemoveFOut(const std::string& file);
private:
	static std::unordered_map<std::string, std::ofstream> outs;
};