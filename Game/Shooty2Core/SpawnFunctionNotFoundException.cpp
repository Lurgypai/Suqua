#include "SpawnFunctionNotFoundException.h"

SpawnFunctionNotFoundException::SpawnFunctionNotFoundException(const std::string& tag) :
    msg{"Unable to find spawn function for entity tag " + tag + '.'}
{}

const char* SpawnFunctionNotFoundException::what() const noexcept {
    return msg.c_str();
}
