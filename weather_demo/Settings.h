
#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

class Settings {
public:
    static std::string temperatureUnit;  // Holds 'C' for Celsius or 'F' for Fahrenheit

    static void setTemperatureUnit(const std::string& unit);
    static std::string getTemperatureUnit();
};

#endif