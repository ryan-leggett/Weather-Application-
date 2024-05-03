// Settings.cpp
#include "Settings.h"

std::string Settings::temperatureUnit = "C";  // Default to Celsius

void Settings::setTemperatureUnit(const std::string& unit) {
    temperatureUnit = unit;
}

std::string Settings::getTemperatureUnit() {
    return temperatureUnit;
}
