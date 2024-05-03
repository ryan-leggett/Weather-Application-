#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <map>
#include <limits>

#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"

#include "LocationManager.h"
#include "WeatherData.h"

#include <curl/curl.h>  // For HTTP requests
#include <nlohmann/json.hpp>
using json = nlohmann::json;


using namespace std;

std::stringstream get_response(std::string_view url)
{
    std::stringstream str;
    curl::curl_ios<std::stringstream> writer(str);

    curl::curl_easy easy(writer);

    easy.add<CURLOPT_URL>(url.data());
    easy.add<CURLOPT_FOLLOWLOCATION>(1L);
    try
    {
        easy.perform();
    }
    catch (curl::curl_easy_exception error)
    {
        auto errors = error.get_traceback();
        error.print_traceback();
    }

    return str;
}





int main()
{
    using namespace std::string_literals;

    auto url = "https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current=temperature_2m,wind_speed_10m&hourly=temperature_2m,relative_humidity_2m,wind_speed_10m";

    auto json = get_response(url);

    //std::cout << json.str() << std::endl;

    LocationManager locManager;
    WeatherData weatherData;

    int choice;

    while (true) {
        cout << "\nMenu:" << endl;
        cout << "1. Manage Locations" << endl;
        cout << "2. Search for Weather Data" << endl;
        cout << "3. Quit" << endl;
        cout << "Enter your choice (1-3): ";
        cin >> choice;

        switch (choice) {
        case 1:
            locManager.manageLocations();
            break;
        case 2:
            weatherData.searchForWeatherData();
            break;
        case 3:
            cout << "Quitting the program..." << endl;
            return 0;
        default:
            cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
        }
    }

    return 0;
}