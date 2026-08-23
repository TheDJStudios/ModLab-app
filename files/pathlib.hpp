#pragma once

#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace pathlib {

class Path {
public:
    Path() = default;
    Path(const char* value) : path_(value) {}
    Path(const std::string& value) : path_(value) {}
    Path(std::filesystem::path value) : path_(std::move(value)) {}

    Path operator/(const Path& other) const { return Path(path_ / other.path_); }
    Path operator/(const std::string& other) const { return Path(path_ / other); }
    Path operator/(const char* other) const { return Path(path_ / other); }

    std::string string() const { return path_.string(); }
    const std::filesystem::path& native() const { return path_; }
    const char* c_str() const { return path_.c_str(); }

    std::string name() const { return path_.filename().string(); }
    std::string stem() const { return path_.stem().string(); }
    std::string suffix() const { return path_.extension().string(); }
    Path parent() const { return Path(path_.parent_path()); }

    bool exists() const { return std::filesystem::exists(path_); }
    bool is_file() const { return std::filesystem::is_regular_file(path_); }
    bool is_dir() const { return std::filesystem::is_directory(path_); }
    bool is_absolute() const { return path_.is_absolute(); }
    bool empty() const { return path_.empty(); }

    Path absolute() const { return Path(std::filesystem::absolute(path_)); }
    Path resolve() const { return Path(std::filesystem::weakly_canonical(path_)); }

    void mkdir(bool parents = false, bool exist_ok = false) const {
        std::error_code error;
        const bool created = parents ? std::filesystem::create_directories(path_, error)
                                     : std::filesystem::create_directory(path_, error);
        if (error && !(exist_ok && exists())) throw std::filesystem::filesystem_error("mkdir", path_, error);
        if (!created && !exist_ok && exists()) throw std::runtime_error("Path already exists: " + string());
    }

    void touch(bool exist_ok = true) const {
        if (!exist_ok && exists()) throw std::runtime_error("Path already exists: " + string());
        std::ofstream file(path_, std::ios::app);
        if (!file) throw std::runtime_error("Could not touch: " + string());
    }

    std::string read_text() const {
        std::ifstream file(path_, std::ios::binary);
        if (!file) throw std::runtime_error("Could not read: " + string());
        std::ostringstream contents;
        contents << file.rdbuf();
        return contents.str();
    }

    void write_text(const std::string& text) const {
        std::ofstream file(path_, std::ios::binary | std::ios::trunc);
        if (!file) throw std::runtime_error("Could not write: " + string());
        file << text;
    }

    void unlink(bool missing_ok = false) const {
        std::error_code error;
        const bool removed = std::filesystem::remove(path_, error);
        if (error || (!removed && !missing_ok)) throw std::filesystem::filesystem_error("unlink", path_, error);
    }

    Path rename(const Path& destination) const {
        std::filesystem::rename(path_, destination.path_);
        return destination;
    }

    std::vector<Path> iterdir() const {
        std::vector<Path> entries;
        for (const auto& entry : std::filesystem::directory_iterator(path_)) entries.emplace_back(entry.path());
        return entries;
    }

    std::vector<Path> glob(const std::string& pattern) const {
        std::vector<Path> matches;
        const std::filesystem::path wanted(pattern);
        const auto extension = wanted.extension();
        for (const auto& entry : std::filesystem::directory_iterator(path_)) {
            if (pattern == "*" || entry.path().filename() == wanted ||
                (pattern.rfind("*.", 0) == 0 && entry.path().extension() == extension)) {
                matches.emplace_back(entry.path());
            }
        }
        return matches;
    }

    static Path cwd() { return Path(std::filesystem::current_path()); }
    static Path home() {
        const char* value = std::getenv("HOME");
        if (!value) throw std::runtime_error("HOME is not set");
        return Path(value);
    }

private:
    std::filesystem::path path_;
};

} // namespace pathlib
