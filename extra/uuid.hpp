#pragma once

#include <QUuid>

#include <algorithm>
#include <stdexcept>
#include <string>

namespace uuid {

class UUID {
public:
    UUID() = default;
    explicit UUID(const std::string& value)
        : value_(QUuid(QString::fromStdString(value))) {
        if (value_.isNull() && value != "00000000-0000-0000-0000-000000000000" &&
            value != "{00000000-0000-0000-0000-000000000000}") {
            throw std::invalid_argument("Invalid UUID: " + value);
        }
    }
    explicit UUID(QUuid value) : value_(std::move(value)) {}

    std::string str() const {
        return value_.toString(QUuid::WithoutBraces).toStdString();
    }
    std::string hex() const {
        std::string result = str();
        result.erase(std::remove(result.begin(), result.end(), '-'), result.end());
        return result;
    }
    int version() const { return static_cast<int>(value_.version()); }
    bool is_nil() const { return value_.isNull(); }
    const QUuid& native() const { return value_; }

    bool operator==(const UUID& other) const { return value_ == other.value_; }
    bool operator!=(const UUID& other) const { return !(*this == other); }

private:
    QUuid value_;
};

inline UUID uuid4() { return UUID(QUuid::createUuid()); }

inline UUID uuid3(const UUID& name_space, const std::string& name) {
    return UUID(QUuid::createUuidV3(name_space.native(), QByteArray::fromStdString(name)));
}

inline UUID uuid5(const UUID& name_space, const std::string& name) {
    return UUID(QUuid::createUuidV5(name_space.native(), QByteArray::fromStdString(name)));
}

inline const UUID NAMESPACE_DNS("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
inline const UUID NAMESPACE_URL("6ba7b811-9dad-11d1-80b4-00c04fd430c8");
inline const UUID NAMESPACE_OID("6ba7b812-9dad-11d1-80b4-00c04fd430c8");

} // namespace uuid
