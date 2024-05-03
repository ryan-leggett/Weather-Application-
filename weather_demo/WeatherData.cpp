#include "WeatherData.h"
#include <iostream>
#include <curl/curl.h>
#include <sstream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <vector>

using json = nlohmann::json;

using namespace std;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

size_t WeatherData::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void WeatherData::weatherForecast() {
    double latitude, longitude;

    // Ask user for latitude and longitude
    cout << "Enter latitude: "; 
    cin >> latitude; //Storing it in variable
    cout << "Enter longitude: ";
    cin >> longitude;

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init(); // Initiating curl API
    if (curl) { 
        std::stringstream url;
        url << "https://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude // Using the Latitude and Longitude to find the correct forecast
            << "&longitude=" << longitude
            << "&hourly=temperature_2m,relative_humidity_2m,precipitation,surface_pressure"
            << "&current_weather=true";  // Add current_weather to get day/night info

        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto result = json::parse(readBuffer);
            if (result.contains("error") && result["error"].get<bool>()) {
                std::cout << "Error fetching data: " << result["reason"] << std::endl;
            }
            else if (result.contains("current_weather")) {
                auto weather = result["current_weather"];
                auto is_day = weather["time"].get<std::string>().find("T") != std::string::npos;

                std::cout << "Current Weather Forecast:" << std::endl;
                std::cout << "Temperature: " << weather["temperature"] << "°C" << std::endl;
                std::cout << "Humidity: " << weather["humidity"] << "%" << std::endl;
                std::cout << "Day/Night: " << (is_day ? "Day" : "Night") << std::endl;
                std::cout << "Rain: " << (weather.contains("precipitation") ? weather["precipitation"].get<float>() : 0) << " mm" << std::endl;
                std::cout << "Surface Pressure: " << weather["pressure"] << " hPa" << std::endl;
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

void WeatherData::hourlyWeather(double latitude, double longitude) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::stringstream url;
        url << "https://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude
            << "&longitude=" << longitude
            << "&hourly=temperature_2m,precipitation,pressure_msl,surface_pressure,wind_speed_10m,relative_humidity_2m,cloud_cover";

        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto result = nlohmann::json::parse(readBuffer);
            if (result.contains("error") && result["error"].get<bool>()) {
                std::cout << "Error fetching data: " << result["reason"] << std::endl;
            }
            else {
                std::cout << "Hourly Weather Data:" << std::endl;
                const int colWidth = 15; // Set column width
                // Print headers with extra spaces for separation
                std::cout << std::left
                    << std::setw(colWidth) << "Hour " // Extra space after "Hour"
                    << std::setw(colWidth) << "Temperature "
                    << std::setw(colWidth) << "Precipitation "
                    << std::setw(colWidth) << "Pressure MSL "
                    << std::setw(colWidth) << "Surface Pressure "
                    << std::setw(colWidth) << "Wind Speed "
                    << std::setw(colWidth) << "Humidity "
                    << std::setw(colWidth) << "Cloudiness" << std::endl;

                for (size_t i = 0; i < result["hourly"]["temperature_2m"].size(); i++) {
                    std::cout << std::setw(colWidth) << i << " "
                        << std::setw(colWidth) << result["hourly"]["temperature_2m"][i] << "°C "
                        << std::setw(colWidth) << result["hourly"]["precipitation"][i] << " mm "
                        << std::setw(colWidth) << result["hourly"]["pressure_msl"][i] << " hPa "
                        << std::setw(colWidth) << result["hourly"]["surface_pressure"][i] << " hPa "
                        << std::setw(colWidth) << result["hourly"]["wind_speed_10m"][i] << " m/s "
                        << std::setw(colWidth) << result["hourly"]["relative_humidity_2m"][i] << "% "
                        << std::setw(colWidth) << result["hourly"]["cloud_cover"][i] << "%" << std::endl;
                }
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

void WeatherData::historicalWeather() {
    cout << "Retrieving historical weather data..." << endl;
    // Implementation details
}

void WeatherData::dailyWeather() {
    double latitude, longitude;

    // Ask user for latitude and longitude
    cout << "Enter latitude: ";
    cin >> latitude;
    cout << "Enter longitude: ";
    cin >> longitude;

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::stringstream url;
        url << "https://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude
            << "&longitude=" << longitude
            << "&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset"
            << "&timezone=auto"; // Automatically determine the timezone for sunrise/sunset times

        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto result = json::parse(readBuffer);
            if (result.contains("error") && result["error"].get<bool>()) {
                std::cout << "Error fetching data: " << result["reason"] << std::endl;
            }
            else if (result.contains("daily")) {
                auto daily = result["daily"];
                std::cout << "Daily Weather Data:" << std::endl;
                std::cout << "Maximum Temperature: " << daily["temperature_2m_max"][0] << "°C" << std::endl;
                std::cout << "Minimum Temperature: " << daily["temperature_2m_min"][0] << "°C" << std::endl;
                std::cout << "Sunrise: " << daily["sunrise"][0] << std::endl;
                std::cout << "Sunset: " << daily["sunset"][0] << std::endl;
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

void WeatherData::solarRadiationForecast(double latitude, double longitude) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::stringstream url;
        // Constructing the API request URL with the specified solar variables
        url << "https://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude
            << "&longitude=" << longitude
            << "&hourly=shortwave_radiation,direct_normal_irradiance,global_tilted_irradiance";

        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto result = json::parse(readBuffer);
            if (result.contains("error") && result["error"].get<bool>()) {
                std::cout << "Error fetching data: " << result["reason"] << std::endl;
            }
            else {
                if (result.contains("hourly")) {
                    auto hourlyData = result["hourly"];
                    std::cout << "Solar Radiation Data:" << std::endl;
                    std::cout << "Shortwave Radiation: "
                        << hourlyData["shortwave_radiation"][0] << " W/m²" << std::endl;
                    std::cout << "Direct Normal Irradiance: "
                        << hourlyData["direct_normal_irradiance"][0] << " W/m²" << std::endl;
                    std::cout << "Global Tilted Irradiance: "
                        << hourlyData["global_tilted_irradiance"][0] << " W/m²" << std::endl;
                }
                else {
                    std::cout << "Solar radiation data not found in the response." << std::endl;
                }
            }
        }
        else {
            std::cout << "Failed " << curl_easy_strerror(res) << std::endl;
        }
    }
    else {
        std::cout << "Failed" << std::endl;
    }
}

void WeatherData::pressureForecast(double latitude, double longitude) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::stringstream url;
        // Constructing the API request URL with the specified pressure variables
        url << "https://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude
            << "&longitude=" << longitude
            << "&hourly=geopotential_height_1000hPa,geopotential_height_925hPa,"
            << "geopotential_height_500hPa,geopotential_height_250hPa,geopotential_height_70hPa";

        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto result = json::parse(readBuffer);
            if (result.contains("error") && result["error"].get<bool>()) {
                std::cout << "Error fetching data: " << result["reason"] << std::endl;
            }
            else {
                // Parsing the pressure data
                // Check the actual structure of the API response and modify the keys accordingly
                if (result.contains("hourly")) {
                    auto hourlyData = result["hourly"];
                    std::cout << "Pressure Data at Various Atmospheric Heights:" << std::endl;
                    std::cout << "Geopotential Height at 1000 hPa: "
                        << hourlyData["geopotential_height_1000hPa"][0] << " gpm" << std::endl;
                    std::cout << "Geopotential Height at 925 hPa: "
                        << hourlyData["geopotential_height_925hPa"][0] << " gpm" << std::endl;
                    std::cout << "Geopotential Height at 500 hPa: "
                        << hourlyData["geopotential_height_500hPa"][0] << " gpm" << std::endl;
                    std::cout << "Geopotential Height at 250 hPa: "
                        << hourlyData["geopotential_height_250hPa"][0] << " gpm" << std::endl;
                    std::cout << "Geopotential Height at 70 hPa: "
                        << hourlyData["geopotential_height_70hPa"][0] << " gpm" << std::endl;
                }
                else {
                    std::cout << "Pressure data not found in the response." << std::endl;
                }
            }
        }
        else {
            std::cout << "Failed " << curl_easy_strerror(res) << std::endl;
        }
    }
    else {
        std::cout << "Failed" << std::endl;
    }
}

void WeatherData::weatherModelForecast(double latitude, double longitude) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::stringstream url;
        // Constructing the API request URL with the specified weather variables and the best matching model
        url << "https://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude
            << "&longitude=" << longitude
            << "&hourly=temperature_2m,relative_humidity_2m,apparent_temperature,"
            << "snowfall,visibility,wind_speed_10m,temperature_80m"
            << "&models=best_match";

        curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto result = json::parse(readBuffer);
            if (result.contains("error") && result["error"].get<bool>()) {
                std::cout << "Error fetching data: " << result["reason"] << std::endl;
            }
            else {
                if (result.contains("hourly")) {
                    auto hourlyData = result["hourly"];
                    std::cout << "Weather Model Forecast (Best Match):" << std::endl;
                    // It's assumed that the API returns a list for each variable,
                    // and here we are just displaying the first set of data as an example.
                    std::cout << "2m Temperature: " << hourlyData["temperature_2m"][0] << " °C" << std::endl;
                    std::cout << "2m Relative Humidity: " << hourlyData["relative_humidity_2m"][0] << " %" << std::endl;
                    std::cout << "Apparent Temperature: " << hourlyData["apparent_temperature"][0] << " °C" << std::endl;
                    std::cout << "Snowfall: " << hourlyData["snowfall"][0] << " mm" << std::endl;
                    std::cout << "Visibility: " << hourlyData["visibility"][0] << " m" << std::endl;
                    std::cout << "10m Wind Speed: " << hourlyData["wind_speed_10m"][0] << " m/s" << std::endl;
                    std::cout << "80m Temperature: " << hourlyData["temperature_80m"][0] << " °C" << std::endl;
                }
                else {
                    std::cout << "Weather model data not found in the response." << std::endl;
                }
            }
        }
        else {
            std::cout << "Failed" << curl_easy_strerror(res) << std::endl;
        }
    }
    else {
        std::cout << "Failed" << std::endl;
    }
}


void WeatherData::searchForWeatherData() {
    int choice;
    double latitude, longitude; // Variables to hold location data

    while (true) {
        cout << "\nWeather Data Menu:" << endl;
        cout << "1 - Weather Forecast" << endl;
        cout << "2 - Historical Weather" << endl;
        cout << "3 - Hourly Weather" << endl; // Added new menu option for hourly weather
        cout << "4 - Daily Weather Forecast" << endl;
        cout << "5 - Solar Variables Forecast" << endl;
        cout << "6 - Pressure Variables Forecast" << endl;
        cout << "7 - Best Weather Model Forecast" << endl;
        cout << "8 - Main Menu" << endl;
        cout << "Enter your choice (1-6): ";
        cin >> choice;

        switch (choice) {
        case 1:
            weatherForecast();
            break;
        case 2:
            historicalWeather();
            break;
        case 3:
            cout << "Enter latitude: ";
            cin >> latitude;
            cout << "Enter longitude: ";
            cin >> longitude;
            hourlyWeather(latitude, longitude); // Call to fetch and display hourly weather
            break;
        case 4:
            dailyWeather();
            break;
        case 5:
            cout << "Enter latitude: ";
            cin >> latitude;
            cout << "Enter longitude: ";
            cin >> longitude;
            solarRadiationForecast(latitude, longitude);
            break;
        case 6:
            cout << "Enter latitude: ";
            cin >> latitude;
            cout << "Enter longitude: ";
            cin >> longitude;
            pressureForecast(latitude, longitude);
        case 7:
            cout << "Enter latitude: ";
            cin >> latitude;
            cout << "Enter longitude: ";
            cin >> longitude;
            weatherModelForecast(latitude, longitude);
        case 8:
            return;  // Return to the main menu
        default:
            cout << "Invalid choice. Please enter a number between 1 and 4." << endl;
        }
    }
}
