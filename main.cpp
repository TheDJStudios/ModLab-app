#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include "files/json.hpp"
#include "files/pathlib.hpp"
#include "windowing/minitk.hpp"
#include "internet/requests.hpp"
#include "extra/color.h"
#include "extra/uuid.hpp"
#include "extra/logger.h"

auto userhome = pathlib::Path::home();
auto approot = userhome / ".Velocity";
auto packroot = approot / "instances";

void init() {
    logs::info("Initializing Velocity");
    if (!approot.exists()) {
        logs::warn("Missing app root. Creating...");
        approot.mkdir(true);
        logs::info("~/.Velocity/ Created!");
    }
    if (!packroot.exists()) {
        logs::warn("Missing pack root. Creating...");
        packroot.mkdir(true);
        logs::info("~/.Velocity/instances/ Created!");
    }


    logs::info("Done initializing. Have fun!");
}

std::vector<std::string> get_minecraft_versions() {
    auto response = requests::get(
        "https://piston-meta.mojang.com/mc/game/version_manifest_v2.json"
    );

    response.raise_for_status();

    const auto manifest = response.json();
    std::vector<std::string> versions;

    for (const auto& version : manifest["versions"].as_array()) {
        if (version["type"].as_string() == "release") {
            versions.push_back(version["id"].as_string());
        }
    }

    return versions;
}

int dlmr(std::string const &id, std::string const &dldir) {
    auto response = requests::get("https://jsonplaceholder.typicode.com/todos/1");
    std::cout << response.text;
    return response.status_code;
};

void make_pack(const std::string& name, const std::string& version, const std::string& loader) {
    logs::info(std::string("Creating Pack ") + name + " for " + version + " " + loader + "...");
    auto pack = packroot / version / loader / name;
    if (!pack.exists()) {
        pack.mkdir(true, true);
    } else {
        logs::warn("Pack already exists!");
        logs::info("Trying to find a work around...");
        pack = packroot / "overflow" / "a" / version / loader / name;
        if (!pack.exists()) {
            pack.mkdir(true, true);
        } else {
            logs::warn("Pack exists in both packs dir and overflow. aborting creation...");
            logs::error("Could not create modpack.");
            return;
        }
    }


    logs::info("pack " + name + " Created!");
};

void delete_pack(const std::string& name, const std::string& version, const std::string& loader, const bool& in_overflow) {
    logs::warn("Removing a modpack..");
    auto pack = packroot / version / loader / name;
    if (pack.exists() && !in_overflow) {
        logs::info("Deleting pack " + name);
        pack.unlink(true);
        logs::info("Pack " + name + " For " + loader + " " + version + " Deleted!");
    }
    if (in_overflow) {
        pack = packroot / "overflow" / "a" / version / loader / name;
        if (pack.exists()) {
            logs::info("Deleting pack " + name);
            pack.unlink(true);
            logs::info("Pack " + name + " For " + loader + " Deleted from overflow!");
        }
    }
    if (!pack.exists()) {
        logs::warn("Pack doesnt exist.");
    }
    pack = packroot / "overflow" / "a" / version / loader / name;
    if (!pack.exists()) {
        logs::info("Pack Doesnt exist in overflow.");
    }
}
void pass() {};

int main(int argc, char *argv[]) {
    init();
    auto root = minitk::Tk(argc, argv);
    auto& content = root.content();
    root.title("Velocity Desktop");
    root.geometry(480, 270);

    minitk::Label title(content, "Velocity");
    title.grid({
        .row = 0,
        .column = 0,
        .padx = 4,
        .pady = 4,
        .align = minitk::Align::top_start
    });

    minitk::Window mp_popup(root, "Modpacks");
    mp_popup.geometry(400, 250);

    minitk::Window create_mp(root, "Create Modpack");
    create_mp.geometry(400, 350);

    minitk::Button mpack(content, "Modpacks", [&] {
        logs::info("Opening modpack window");
        mp_popup.show();
    });

    minitk::Button createmp(mp_popup, "Create Modpack", [&] {
        logs::info("Opening MP Creation window");
        create_mp.show();
    });

    minitk::Button rm_pack(mp_popup, "Remove Modpack", [] {
        logs::info("Removing Pack");
        delete_pack("static_tests", "1.20.1", "vanilla", false);
    });

    minitk::Label mkmpname(create_mp, "Name");
    mkmpname.grid({
        .row = 0,
        .column = 0,
        .padx = 4,
        .pady = 4,
        .align = minitk::Align::top_start
    });

    minitk::Entry mkmpnameentry(create_mp);
    mkmpnameentry.grid({
    .row = 0,
    .column = 1,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    minitk::Label mkmploaderlabel(create_mp, "loader");
    mkmploaderlabel.grid({
    .row = 1,
    .column = 0,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    minitk::Combobox mkmp_loader(create_mp, {
        "vanilla",
        "forge",
        "fabric",
        "neoforge",
        "quilt"
    });

    mkmp_loader.grid({
    .row = 1,
    .column = 1,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    minitk::Label mkmp_version_label(create_mp, "version");
    mkmp_version_label.grid({
    .row = 2,
    .column = 0,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    minitk::Combobox mkmp_version(create_mp, {get_minecraft_versions()});
    mkmp_version.grid({
    .row = 2,
    .column = 1,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    minitk::Button mk_pack(create_mp, "Create Modpack", [&] {
        std::string name;
        std::string loader;
        name = mkmpnameentry.get();
        loader = mkmp_loader.get();


        make_pack(name, mkmp_version.get(), loader);
        create_mp.close();

    });

    mk_pack.grid({
    .row = 3,
    .column = 0,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::center
    });

    mpack.grid({
    .row = 0,
    .column = 2,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    rm_pack.grid({
    .row = 0,
    .column = 0,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    createmp.grid({
    .row = 0,
    .column = 1,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    return root.mainloop();
}

