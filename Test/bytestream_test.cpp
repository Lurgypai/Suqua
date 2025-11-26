#include <print>

#include "ByteStream.h"
#include "UUID.h"

int main() {
    int x = 13;
    std::string s = "hello, world!";
    Suqua::UUID uuid = Suqua::UUID::GenerateUUID();

    ByteStream stream;
    stream << x;

    auto sizePos = stream.getPos();
    stream.allocateData(sizeof(size_t));

    stream << s;
    stream << uuid;

    size_t size = 2;

    size_t end = stream.getPos();
    stream.setPos(sizePos);
    stream << size;
    stream.setPos(end);

    stream.setPos(0);
    int rX;
    size_t rSize;
    std::string rS;
    Suqua::UUID rUuid;

    stream >> rX;
    stream >> rSize;
    stream >> rS;
    stream >> rUuid;

    std::println("{} == {}? {}", x, rX, x == rX);
    std::println("{} == {}? {}", size, rSize, size == rSize);
    std::println("{} == {}? {}", s, rS, s == rS);
    std::println("{} == {}? {}", uuid.str(), rUuid.str(), uuid == rUuid);

    return 0;
}
