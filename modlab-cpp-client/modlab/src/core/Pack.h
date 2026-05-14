#pragma once
#include <QString>

// Loader type
enum class Loader { Vanilla, Fabric, Forge };

inline QString loaderName(Loader l) {
    switch (l) {
        case Loader::Fabric:  return "Fabric";
        case Loader::Forge:   return "Forge";
        case Loader::Vanilla: return "Vanilla";
    }
    return "Vanilla";
}

inline QString loaderAbbr(Loader l) {
    switch (l) {
        case Loader::Fabric:  return "FBR";
        case Loader::Forge:   return "FRG";
        case Loader::Vanilla: return "VNL";
    }
    return "VNL";
}

// Pack status
enum class PackStatus { Ready, Installing, NeedsUpdate, NotInstalled };

inline QString statusLabel(PackStatus s) {
    switch (s) {
        case PackStatus::Ready:        return "Ready";
        case PackStatus::Installing:   return "Installing";
        case PackStatus::NeedsUpdate:  return "Update";
        case PackStatus::NotInstalled: return "Not Installed";
    }
    return "";
}

// A single pack/instance
struct Pack {
    QString key;           // lowercase unique ID used by Python backend
    QString name;          // display name
    QString mcVersion;     // e.g. "1.21.4"
    Loader  loader;
    int     modCount  = 0;
    PackStatus status = PackStatus::NotInstalled;
    int     installProgress = 0;  // 0-100, relevant during Installing

    // 3-letter abbreviation for the ident tile
    QString abbr() const {
        return name.length() >= 3
            ? name.left(3).toUpper()
            : name.toUpper();
    }
};
