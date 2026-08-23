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


int main(int argc, char *argv[]) {
    auto root = minitk::Tk(argc, argv);
    root.title("Velocity Desktop");
    root.geometry(480, 270);

    return root.mainloop();
}

