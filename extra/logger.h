#pragma once
#include "color.h"
#include <iostream>
#ifndef VELOCITY_APP_LOGGER_H
#define VELOCITY_APP_LOGGER_H

namespace logger {
    inline void info(std::string const &message) {std::cout << "[Velocity] INFO " << message << std::endl;};
    inline void warn(std::string const &message) {std::cout << YELLOW << "[Velocity] WARN " << message << RESET << std::endl;};
    inline void error(std::string const &message) {std::cout << RED << "[Velocity] ERROR " << message << RESET << std::endl;};
}// namespace logger


#endif //VELOCITY_APP_LOGGER_H
