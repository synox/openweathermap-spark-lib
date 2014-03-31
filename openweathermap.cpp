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
			+ "&mode=json" // xml or json
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
 *
 */
weather_response_t Weather::parse(String data) {
	/*
	 * example:
	 * http://api.openweathermap.org/data/2.5/forecast/daily?q=London,uk&units=metric&cnt=1
	 *
	 * {"cod":"200","message":0.3348,
	 * "city":{"id":2643743,"name":"London",
	 * "coord":{"lon":-0.12574,"lat":51.50853},
	 * "country":"GB","population":0,"sys":{"population":0}},"cnt":1,
	 * "list":[{"dt":1396267200,
	 * "temp":{"day":14.69,"min":8.6,"max":14.69,"night":8.6,"eve":14.69,"morn":14.69},
	 * "pressure":1019.37,"humidity":66,
	 * "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04d"}],
	 * "speed":3.7,"deg":162,"clouds":64}]}
	 */
	weather_response_t response;
	unsigned char buffer[600];
	data.getBytes(buffer, sizeof(buffer), 0);
	JsonHashTable root = parser.parseHashTable((char*) buffer);
	if (!root.success()) {
		Serial.println(
				"Parsing fail: could be an invalid JSON, or too many tokens");
		return response;
	}

	JsonArray daysList = root.getArray("list");
	JsonHashTable today = daysList.getHashTable(0);
	JsonHashTable temp = today.getHashTable("temp");
	response.temp_low = temp.getLong("min");
	response.temp_high = temp.getLong("max");

	JsonHashTable weather = today.getArray("weather").getHashTable(0);
	response.descr = weather.getString("description");
	response.conditionCode = weather.getLong("id");

	// a value over 250 degree/fahrenheit makes no sense
	response.isSuccess = true;
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
