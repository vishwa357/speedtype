#pragma once
#ifdef DEBUG
#include <filesystem>
#include <functional>
#include <string>

class HotReload {
public:
    using Callback = std::function<void()>;

    HotReload(const std::string& path, Callback cb);
    void Poll();

private:
    std::string path;
    Callback    callback;
    std::filesystem::file_time_type lastWrite;
};
#endif
