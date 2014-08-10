/**
 * This is a library for the openweathermap.org api, made for spark core (http://spark.io).
 *
 * It uses the API described at http://openweathermap.org/API
 */

#ifndef _SPARKWEATHER
#define _SPARKWEATHER

#include "application.h"
#include "JsonParser.h"
#include "HttpClient.h"

typedef struct weather_response_t {
	long temp_high;
	long temp_low;
	String descr;
	long conditionCode; // see http://openweathermap.org/wiki/API/Weather_Condition_Codes
	bool isSuccess;
	// defaults:
	weather_response_t(): temp_high(255), temp_low(255), conditionCode(-1), isSuccess(false) {};
} weather_response_t;

class Weather {
public:
	Weather(String location, HttpClient* client, String apiKey);
	bool update(weather_response_t& response);
	void setCelsius();
	void setFahrenheit();

	// for cache:
	weather_response_t cachedUpdate();

private:
	ArduinoJson::Parser::JsonParser<70> parser; // occupies 70 * 4 bytes


	http_request_t request;
	String location;
	String apiKey;
	String unitsForTemperature;
	HttpClient* client;
	bool parse(String& json, weather_response_t& response);

	// cache:
	unsigned long weather_sync_interval;
	unsigned long lastsync;
	weather_response_t lastReponse;
};

#endif
