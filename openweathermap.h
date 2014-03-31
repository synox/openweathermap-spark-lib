/**
 * This is a library for the openweathermap.org api, made for spark core (http://spark.io).
 *
 * It uses the API described at http://openweathermap.org/API
 */

#ifndef _SPARKWEATHER
#define _SPARKWEATHER

#include "application.h"
#include "HttpClient.h"


typedef struct weather_response_t {
	int temp_high;
	int temp_low;
	String descr;
	int conditionCode; // see http://openweathermap.org/wiki/API/Weather_Condition_Codes
	bool isSuccess;
	// defaults:
	weather_response_t(): temp_high(255), temp_low(255), conditionCode(-1), isSuccess(false) {};
} weather_response_t;

class Weather {
public:
	Weather(String location, HttpClient* client, String apiKey);
	weather_response_t update();
	void setCelsius();
	void setFahrenheit();

	// for cache:
	weather_response_t cachedUpdate();

private:
	http_request_t request;
	String location;
	String apiKey;
	String unitsForTemperature;
	HttpClient* client;
	weather_response_t response;
	weather_response_t parse(String json);
	String readData(String s, String needle, String endMark);
	int readIntField(const char* fieldname, String jsonData);
	String readTextField(const char* fieldname, String jsonData);

	long parseInt(String s);

	// cache:
	unsigned long weather_sync_interval;
	unsigned long lastsync;
	weather_response_t lastReponse;

};

#endif
