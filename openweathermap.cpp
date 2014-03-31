#include "openweathermap.h"

/** Show temp in degree celsius */
void Weather::setCelsius() {
	this->unitsForTemperature = "metric";
}
void Weather::setFahrenheit() {
	this->unitsForTemperature = "imperial";
}

Weather::Weather(String location, HttpClient* client, String apiKey) {
	this->location = location;
	this->client = client;
	this->apiKey = apiKey;

	// default:
	setCelsius();

	// init cache
	this->lastsync = 0;
	this->weather_sync_interval = 1000 * 3600 * 2; // milliseconds

}

weather_response_t Weather::update() {
	Serial.print("loading weather with url: ");
	Serial.println(request.path);

	request.hostname = "api.openweathermap.org";
	request.port = 80;
	request.path = "/data/2.5/forecast/daily?q=" //
			+ location // e.g. "Berlin,de"
			+ "&units=" + unitsForTemperature // metric or imperial
			+ "&cnt=1" // number of days
			+ "&mode=xml" // xml or json
			+ "&APPID=" + apiKey; // see http://openweathermap.org/appid
	request.body = "";

	http_response_t response;
	this->client->get(request, response);
	if (response.status == 200) {
		return parse(response.body);
	} else {
		Serial.print("weather request failed ");
		weather_response_t responseFailed;
		return responseFailed;
	}
}

/**
 * In the String data, it searches for a needle, and returns the string after the needle until the endMark.
 *
 * Example: readData(' ,min="34", ', '"high":"', '"') = 34
 */
String Weather::readData(String data, String needle, String endMark) {
	int begin = data.indexOf(needle) + needle.length();
	int end = data.indexOf(endMark, begin + 1);

	if(begin < 5) {
		Serial.print("can not find string: ");
		Serial.println(needle);
		return "???";
	}
	String result = data.substring(begin, end);
	Serial.print("searching:");
	Serial.print(needle);
	Serial.print(", endmark:");
	Serial.print(endMark);
	Serial.print(", result:");
	Serial.println(result);
	return result;
}

int Weather::readIntField(const char* fieldname, String jsonData) {
	return parseInt(readTextField(fieldname, jsonData));
}

String Weather::readTextField(const char* fieldname, String jsonData) {
	String searchString(fieldname);
	searchString += "=\"";
	String result = readData(jsonData, searchString, "\"");

	Serial.print("searching ");
	Serial.print(searchString);
	Serial.print(", found ");
	Serial.println(result);
	return result;
}



long Weather::parseInt(String s) {
	if (s != NULL) {
		return s.toInt();
	} else {
		return -99;
	}
}

weather_response_t Weather::parse(String data) {
	weather_response_t response;
	response.temp_high = readIntField("max", data);
	response.temp_low = readIntField("min", data);
	response.descr = readTextField("name", data);
	response.conditionCode = readIntField("number", data);

	// a value over 250 degree/fahrenheit makes no sense
	response.isSuccess = response.temp_low < 250 && response.descr.length() > 0;
	return response;
}

/**
 * Reads from the cache if there is a fresh and valid response.
 */
weather_response_t Weather::cachedUpdate() {
	if (lastsync == 0 || (lastsync + weather_sync_interval) < millis()) {
		lastReponse = this->update();
		if (lastReponse.isSuccess) {
			lastsync = millis();
		}
	} else {
		Serial.println("using cached weather");
	}
	return lastReponse;
}

// http example:

// http://api.openweathermap.org/data/2.5/forecast/daily?q=Bern,ch&mode=xml&units=metric&cnt=1
//
//<?xml version="1.0" encoding="utf-8"?>
//<weatherdata>
//  <location>
//    <name>Berne</name>
//    <type/>
//    <country>Switzerland</country>
//    <timezone/>
//    <location altitude="0" latitude="46.9484" longitude="7.44046" geobase="geonames" geobaseid="0"/>
//  </location>
//  <credit/>
//  <meta>
//    <lastupdate/>
//    <calctime>0.1996</calctime>
//    <nextupdate/>
//  </meta>
//  <sun rise="2014-03-31T05:10:16" set="2014-03-31T17:58:17"/>
//  <forecast>
//    <time day="2014-03-31">
//      <symbol number="801" name="few clouds" var="02d"/>
//      <precipitation/>
//      <windDirection deg="273" code="W" name="West"/>
//      <windSpeed mps="2.41" name="Light breeze"/>
//      <temperature day="16.66" min="6.54" max="16.66" night="6.54" eve="12.74" morn="16.66"/>
//      <pressurHttpClient>
