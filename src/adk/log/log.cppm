export module adk.log;

import std.io;

export void
TestLog(const std::string &msg)
{
    std::cout << "Test log: " << msg << "\n";
}
