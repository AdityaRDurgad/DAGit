#include "core/object_store.hpp"
#include "core/repository.hpp"

#include <openssl/sha.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace fs = std::filesystem;

namespace core {

std::string ObjectStore::hash_object(const std::string& type, const std::string& data) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) {
        throw std::runtime_error("Fatal: Not a dagit repository.");
    }

    // 1. Construct internal format: <type>\0<data>
    std::string store = type + '\0' + data;

    // 2. Generate SHA1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(store.c_str()), store.length(), hash);

    // 3. Convert to hex string
    std::stringstream hex_stream;
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    std::string oid = hex_stream.str();

    // 4. Store object (using the flat <oid> format per the guide)
    fs::path object_path = *repo_root / ".dagit" / "objects" / oid;
    
    // 5. Immutable storage
    if (!fs::exists(object_path)) {
        std::ofstream out_file(object_path, std::ios::binary);
        out_file.write(store.c_str(), store.length());
    }

    return oid;
}

std::string ObjectStore::get_object(const std::string& oid, const std::string& expected_type) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) throw std::runtime_error("Fatal: Not a dagit repository.");

    fs::path object_path = *repo_root / ".dagit" / "objects" / oid;
    if (!fs::exists(object_path)) {
        throw std::runtime_error("Fatal: object not found " + oid);
    }

    // Read raw bytes
    std::ifstream in_file(object_path, std::ios::binary);
    std::stringstream buffer;
    buffer << in_file.rdbuf();
    std::string raw_data = buffer.str();

    // Split at '\0'
    size_t null_pos = raw_data.find('\0');
    if (null_pos == std::string::npos) {
        throw std::runtime_error("Fatal: corrupted object " + oid);
    }

    std::string type = raw_data.substr(0, null_pos);
    std::string content = raw_data.substr(null_pos + 1);

    // Validate type
    if (!expected_type.empty() && type != expected_type) {
        throw std::runtime_error("Fatal: expected " + expected_type + " but found " + type);
    }

    return content;
}

bool ObjectStore::object_exists(const std::string& oid) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) return false;
    return fs::exists(*repo_root / ".dagit" / "objects" / oid);
}

} // namespace core 