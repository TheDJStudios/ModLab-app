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
    logger::info("Initializing Velocity");
    if (!approot.exists()) {
        logger::warn("Missing app root. Creating...");
        approot.mkdir(true);
        logger::info("~/.Velocity/ Created!");
    }
}



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


    return root.mainloop();
}

