#pragma once
#include "reallife_data.hpp"
#include "content.hpp"
inline std::string realLifeJson(const std::string& topic) {
    auto& m = realLifeMap(); auto it = m.find(topic);
    if (it == m.end()) return "{\"available\":false}";
    std::string j = "{\"available\":true,\"demo\":" + std::to_string(it->second.demo) + ",\"uses\":[";
    for (size_t i = 0; i < it->second.uses.size(); ++i) j += (i ? "," : "") + q(it->second.uses[i]);
    return j + "]}";
}
