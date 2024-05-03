#ifndef LOCATION_MANAGER_H
#define LOCATION_MANAGER_H

#include <string>
#include <map>
#include <limits>

struct Location {
    int id;
    std::string name;
    double latitude;
    double longitude;
};

class LocationManager {
private:
    std::map<int, Location> locations;
    std::string tempUnit; // Temperature unit, either 'C' or 'F'

public:
    void addNewLocation();
    void listLocations();
    void modifyLocationData(int id);
    void fetchWeatherData(double latitude, double longitude);
    void searchForLocation();
    void deleteLocation(int id);
    void manageLocations();
    void settingsMenu();
    void setTemperatureUnit(const std::string& unit); // Function to set the temperature unit
    std::string getTemperatureUnit() const; // Function to get the current temperature unit
};

#endif // LOCATION_MANAGER_H
