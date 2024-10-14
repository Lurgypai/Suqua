#pragma once
#include <exception>
#include <string>

class SpawnFunctionNotFoundException : public std::exception {
public:
    SpawnFunctionNotFoundException(const std::string& tag);
    virtual const char* what() const noexcept override;
private:
    std::string msg;
};
