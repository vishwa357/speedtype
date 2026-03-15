#ifdef DEBUG
#include "HotReload.hpp"

HotReload::HotReload(const std::string& path, Callback cb)
    : path(path), callback(cb) {
    lastWrite = std::filesystem::last_write_time(path);
}

void HotReload::Poll() {
    auto current = std::filesystem::last_write_time(path);
    if (current != lastWrite) {
        lastWrite = current;
        callback();
    }
}
#endif
