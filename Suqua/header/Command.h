#pragma once
#include <string>
#include <vector>

class Command {
public:
	virtual std::string getTag() const = 0;
	virtual void onCommand(const std::vector<std::string>& args) = 0;
};