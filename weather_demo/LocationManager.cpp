#include "LocationManager.h"
#include <iostream>
#include <sstream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <limits>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

//------------------------------------------------------------------------------------------------------------

void LocationManager::addNewLocation() {
    int id;
    std::string name;
    double latitude, longitude;

    std::cout << "Enter ID: ";
    std::cin >> id;
    std::cin.ignore(10000, '\n');  // Clear the input buffer

    std::cout << "Enter name: ";
    getline(std::cin, name);

    std::cout << "Enter latitude: ";
    std::cin >> latitude;
    std::cin.ignore(10000, '\n');  // Clear the input buffer

    std::cout << "Enter longitude: ";
    std::cin >> longitude;
    std::cin.ignore(10000, '\n');  // Clear the input buffer

    std::ofstream outFile;
    // Hardcoded filename "locations.csv"
    outFile.open("locations.csv", std::ios::out | std::ios::app);  // Open file in append mode
    if (!outFile) {
        std::cout << "Failed to open file: locations.csv" << std::endl;
        return;
    }

    // Check if the file is empty and headers need to be added
    outFile.seekp(0, std::ios::end);  // Go to the end of the file
    if (outFile.tellp() == 0) {  // File position at end will be 0 if file is empty
        outFile << "ID, Name, Latitude, Longitude\n";  // Write headers if file is empty
    }

    // Write the location data to the file with proper CSV formatting
    outFile << id << ", \"" << name << "\", " << latitude << ", " << longitude << "\n";
    outFile.close();  // Close the file

    std::cout << "Location added/updated: " << name << std::endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------


void LocationManager::listLocations() {
    std::ifstream inFile("locations.csv");
    if (!inFile.is_open()) {
        std::cout << "Failed to open file: locations.csv" << std::endl;
        return;
    }

    std::string line;
    int lineCount = 0;

    std::cout << "Listing all locations:" << std::endl;
    while (getline(inFile, line)) {
        // Skip the header line
        if (lineCount == 0) {
            lineCount++;
            continue;
        }

        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> fields;

        // Split the line by commas, considering potential commas within quotes
        while (getline(ss, field, ',')) {
            size_t start = field.find_first_not_of(' ');  // Trim leading whitespace
            size_t end = field.find_last_not_of(' ');     // Trim trailing whitespace
            field = field.substr(start, (end - start + 1));
            // Check for and remove quotes
            if (field.front() == '"' && field.back() == '"') {
                field = field.substr(1, field.size() - 2);
            }
            fields.push_back(field);
        }

        if (fields.size() >= 4) { // Check if all fields are present
            std::cout << "ID: " << fields[0] << ", Name: " << fields[1]
                << ", Latitude: " << fields[2] << ", Longitude: " << fields[3] << std::endl;
        }
        lineCount++;
    }

    inFile.close(); // Close the file
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void LocationManager::modifyLocationData(int id) {
    std::ifstream inFile("locations.csv");
    std::vector<std::string> rows;
    std::string line;
    bool found = false;
    int foundIndex = -1;

    if (!inFile.is_open()) {
        std::cout << "Failed to open file: locations.csv" << std::endl;
        return;
    }

    // Read all lines from the file
    while (getline(inFile, line)) {
        rows.push_back(line);
    }
    inFile.close();

    // Process lines to find the specific location
    for (size_t i = 1; i < rows.size(); i++) {
        std::istringstream ss(rows[i]);
        std::string part;
        getline(ss, part, ',');  // Get the ID part of the row
        if (std::stoi(part) == id) {
            found = true;
            foundIndex = i;
            break;
        }
    }

    if (!found) {
        std::cout << "Location with ID " << id << " not found." << std::endl;
        return;
    }

    std::istringstream ss(rows[foundIndex]);
    std::string itemId, itemName, itemLat, itemLong;
    getline(ss, itemId, ',');
    getline(ss, itemName, ',');
    getline(ss, itemLat, ',');
    getline(ss, itemLong);

    // Ask user for new data
    std::string newName;
    double newLatitude, newLongitude;
    std::cout << "Modifying Location: " << itemName << std::endl;
    std::cout << "Current details: ID: " << itemId << ", Name: " << itemName << ", Latitude: " << itemLat << ", Longitude: " << itemLong << std::endl;

    std::cout << "Enter new name (or press enter to keep \"" << itemName << "\"): ";
    getline(std::cin, newName);
    std::cin.ignore(10000, '\n');  // Clear the input buffer
    if (!newName.empty()) {
        itemName = newName;
    }

    std::cout << "Enter new latitude (or enter current value [" << itemLat << "] to keep it): ";
    std::cin >> newLatitude;
    std::cin.ignore(10000, '\n');  // Clear the input buffer
    if (!std::cin.fail()) {
        itemLat = std::to_string(newLatitude);
    }
    std::cin.clear();

    std::cout << "Enter new longitude (or enter current value [" << itemLong << "] to keep it): ";
    std::cin >> newLongitude;
    std::cin.ignore(10000, '\n');  // Clear the input buffer
    if (!std::cin.fail()) {
        itemLong = std::to_string(newLongitude);
    }
    std::cin.clear();

    // Update the row in the vector
    rows[foundIndex] = itemId + "," + itemName + "," + itemLat + "," + itemLong;

    // Write all rows back to the file
    std::ofstream outFile("locations.csv");
    for (const auto& row : rows) {
        outFile << row << std::endl;
    }
    outFile.close();

    std::cout << "Location updated successfully!" << std::endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void LocationManager::setTemperatureUnit(const std::string& unit) {
    if (unit == "C" || unit == "F") {
        tempUnit = unit;
    }
    else {
        std::cout << "Invalid temperature unit. Please choose 'C' or 'F'." << std::endl;
    }
}

std::string LocationManager::getTemperatureUnit() const {
    return tempUnit;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void LocationManager::settingsMenu() {
    int choice;
    while (true) {
        cout << "\nSettings Menu:" << endl;
        cout << "1. Change Temperature Unit" << endl;
        cout << "2. Back to Manage Locations Menu" << endl;
        cout << "Enter your choice (1-2): ";
        cin >> choice;

        // Check for input failure
        if (cin.fail()) {
            cin.clear(); // Clear error flag
            cin.ignore(10000, '\n');  // Clear the input buffer
            cout << "Invalid input. Please enter a number." << endl;
            continue; // Continue to the next iteration of the loop
        }

        switch (choice) {
        case 1: {
            string unit;
            cout << "Enter temperature unit ('C' for Celsius, 'F' for Fahrenheit): ";
            cin >> unit;
            setTemperatureUnit(unit);
            cout << "Temperature unit set to " << getTemperatureUnit() << endl;
            break;
        }
        case 2:
            return; // Return to the previous menu
        default:
            cout << "Invalid choice. Please enter a number between 1 and 2." << endl;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void LocationManager::fetchWeatherData(double latitude, double longitude) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::stringstream url;
        url << "https://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude
            << "&longitude=" << longitude
            << "&hourly=temperature_2m"
            << "&temperature_unit=" << (getTemperatureUnit() == "F" ? "fahrenheit" : "celsius"); // Adjusting URL based on user preference

        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            json result = json::parse(readBuffer);
            if (result.contains("error") && result["error"].get<bool>()) {
                std::cout << "Error fetching data: " << result["reason"] << std::endl;
            }
            else {
                // Use the temperature unit from the settings for display
                std::cout << "Current Temperature: " << result["hourly"]["temperature_2m"][0] << "°" << getTemperatureUnit() << std::endl;
                // Add other parameters and print them similarly
            }
        }
        else {
            std::cout << "Failed to fetch weather data: " << curl_easy_strerror(res) << std::endl;
        }
    }
    else {
        std::cout << "Failed to initialize cURL." << std::endl;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void LocationManager::searchForLocation() {
    std::string name;
    std::cout << "Enter the name of the location to search for and get the weather forecast: ";

    // Clear any lingering newline or other characters in the input buffer
    if (std::cin.peek() == '\n') {
        std::cin.ignore(10000, '\n');
    }

    std::getline(std::cin, name);

    bool found = false;

    for (const auto& pair : locations) {
        const Location& loc = pair.second;
        if (loc.name == name) {
            std::cout << "Location found: " << std::endl;
            std::cout << "ID: " << loc.id << ", Name: " << loc.name
                << ", Latitude: " << loc.latitude << ", Longitude: " << loc.longitude << std::endl;
            fetchWeatherData(loc.latitude, loc.longitude);
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "No location found with the name \"" << name << "\"." << std::endl;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void LocationManager::deleteLocation(int id) {
    auto it = locations.find(id);
    if (it != locations.end()) {
        locations.erase(it);
        cout << "Location with ID " << id << " has been deleted successfully." << endl;
    }
    else {
        cout << "Location with ID " << id << " not found." << endl;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void LocationManager::manageLocations() {
    int choice;
    string searchQuery;

    while (true) {
        cout << "\nManage Locations Menu:" << endl;
        cout << "1. Add New Location" << endl;
        cout << "2. List of Locations" << endl;
        cout << "3. Modify Location Data" << endl;
        cout << "4. Search for Location" << endl;
        cout << "5. Delete Location" << endl;
        cout << "6. Settings Menu" << endl;
        cout << "7. Main Menu" << endl;
        cout << "Enter a number between 1 and 6 ";
        cin >> choice;

        switch (choice) {
        case 1:
            addNewLocation();  // Assuming this function now handles input itself
            break;
        case 2:
            listLocations();
            break;
        case 3:
            int id;
            cout << "Enter the ID of the location to modify: ";
            cin >> id;
            modifyLocationData(id);
            break;
        case 4:
            searchForLocation();
            break;
        case 5:
            cout << "Enter the ID of the location to delete: ";
            cin >> id;
            deleteLocation(id);
            break;
        case 6:
            settingsMenu();
        case 7:
            return;  // Return to the main menu
        default:
            cout << "Invalid choice. Please enter a number between 1 and 5." << endl;
        }
    }
}