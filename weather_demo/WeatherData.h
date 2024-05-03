#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include "LocationManager.h"
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

class WeatherData {
private:
    LocationManager locManager;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

public:
    void weatherForecast();
    void historicalWeather();
    void searchForWeatherData();
    void hourlyWeather(double latitude, double longitude); // New function for hourly weather
    void dailyWeather();
    void solarRadiationForecast(double latitude, double longitude);
    void pressureForecast(double latitude, double longitude);
    void weatherModelForecast(double latitude, double longitude);


};

#endif // WEATHERDATA_H
