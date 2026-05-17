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

    std::string store = type + '\0' + data;

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(store.c_str()), store.length(), hash);

    std::stringstream hex_stream;
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    std::string oid = hex_stream.str();

    // FIXED: Split the hash into a 2-char directory and 38-char filename
    std::string dir_name = oid.substr(0, 2);
    std::string file_name = oid.substr(2);

    fs::path object_dir = *repo_root / ".dagit" / "objects" / dir_name;
    fs::path object_path = object_dir / file_name;
    
    // Create the 2-char directory if it doesn't exist yet
    fs::create_directories(object_dir);

    if (!fs::exists(object_path)) {
        std::ofstream out_file(object_path, std::ios::binary);
        out_file.write(store.c_str(), store.length());
    }

    return oid;
}

std::string ObjectStore::get_object(const std::string& oid, const std::string& expected_type) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) throw std::runtime_error("Fatal: Not a dagit repository.");

    // FIXED: Look for the file inside its 2-char subfolder
    std::string dir_name = oid.substr(0, 2);
    std::string file_name = oid.substr(2);
    fs::path object_path = *repo_root / ".dagit" / "objects" / dir_name / file_name;

    if (!fs::exists(object_path)) {
        throw std::runtime_error("Fatal: object not found " + oid);
    }

    std::ifstream in_file(object_path, std::ios::binary);
    std::stringstream buffer;
    buffer << in_file.rdbuf();
    std::string raw_data = buffer.str();

    size_t null_pos = raw_data.find('\0');
    if (null_pos == std::string::npos) {
        throw std::runtime_error("Fatal: corrupted object " + oid);
    }

    std::string type = raw_data.substr(0, null_pos);
    std::string content = raw_data.substr(null_pos + 1);

    if (!expected_type.empty() && type != expected_type) {
        throw std::runtime_error("Fatal: expected " + expected_type + " but found " + type);
    }

    return content;
}

bool ObjectStore::object_exists(const std::string& oid) {
    auto repo_root = Repository::find_repo_root();
    if (!repo_root) return false;

    // FIXED: Check existence inside the 2-char subfolder
    if (oid.length() < 4) return false; // Basic safety check
    std::string dir_name = oid.substr(0, 2);
    std::string file_name = oid.substr(2);
    
    return fs::exists(*repo_root / ".dagit" / "objects" / dir_name / file_name);
}

} // namespace core