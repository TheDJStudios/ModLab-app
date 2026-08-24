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

void init() {
    logs::info("Initializing Velocity");
    if (!approot.exists()) {
        logs::warn("Missing app root. Creating...");
        approot.mkdir(true);
        logs::info("~/.Velocity/ Created!");
    }


    logs::info("Done initializing. Have fun!");
}

int dlmr(std::string const &id, std::string const &dldir) {
    auto response = requests::get("https://jsonplaceholder.typicode.com/todos/1");
    std::cout << response.text;
    return response.status_code;
};

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

    minitk::Button mk_pack(content, "Make Modpack", [] {
        logs::info("Creating Pack");
    });
    mk_pack.grid({
    .row = 0,
    .column = 1,
    .padx = 4,
    .pady = 4,
    .align = minitk::Align::top_start});

    dlmr("", "");

    return root.mainloop();
}

