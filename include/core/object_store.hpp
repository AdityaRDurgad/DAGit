#pragma once

#include <string>
#include <optional>

namespace core {

class ObjectStore {
public:
    static std::string hash_object(const std::string& type, const std::string& data);
    static std::string get_object(const std::string& oid, const std::string& expected_type = "");
    static bool object_exists(const std::string& oid);
};

} // namespace core