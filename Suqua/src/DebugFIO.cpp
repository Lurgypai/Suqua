#include "DebugFIO.h"
#include <chrono>
#include <ctime>

std::unordered_map<std::string, std::ofstream> DebugFIO::outs{};

void DebugFIO::AddFOut(const std::string& file) {
	outs.emplace(file, std::ofstream{file, std::ios::app});
}

std::ostream& DebugFIO::Out(const std::string& file)
{
	if (outs.find(file) != outs.end()) {
		auto now = std::chrono::system_clock().now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::string s{};
		s.resize(26); ctime_s(s.data(), s.size(), &time);
		return (outs[file] << '[' << s.substr(0, s.size() - 2) << "] ");
	}

	std::out_of_range e{"Unable to find output file \"" + file + "\""};
	throw e;
}

void DebugFIO::RemoveFOut(const std::string& file) {
	outs.erase(outs.find(file));
}
