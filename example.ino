
#include "openweathermap.h"
#include "HttpClient.h"

unsigned int nextTime = 0;    // next time to contact the server

Weather* weather;
HttpClient* httpClient;

void setup() {
	Serial.begin(9600);

	httpClient = new HttpClient();
	weather = new Weather("London,UK", httpClient,
			"INSERT your api key here!");
	weather->setFahrenheit();
}

void loop() {
	if (nextTime > millis()) {
		// keep the same color while waiting
		return;
	}
	// print weather
	weather_response_t resp = weather->cachedUpdate();
	if (resp.isSuccess) {
		Serial.print(resp.temp_low);
		Serial.print("-");
		Serial.print(resp.temp_high);
		Serial.println(resp.descr);
	}

	// check again in 5 seconds:
	nextTime = millis() + 5000;
}
