Weather Library for Spark-Core
=============

This is a simple library to load weather information from openweathermap.com, made for spark core (http://spark.io) (Arduino compatible).

It's free, you just have to sign up for an api-key at: http://openweathermap.org/appid

> This library should be considered **alpha** tested software. Please report any problems, I am happy to help. 

## Installation
* You have to download the ``HttpClient.cpp``/``HttpClient.h`` files from [HttpClient library](https://github.com/nmattisson/HttpClient) to your project. 
* As well download the library https://github.com/bblanchon/ArduinoJsonParser with the ``.cpp`` and ``.h`` files. Put the files in the ``utility`` directory also in a the ``utility`` directory in your project. 
* Download the files ``.cpp``/``.h`` files from this project page. 


If you are building locally, change the file ``build.mk`` and add the ``.cpp`` files. 


## Usage
See the following code, replace the location and the api key. 

		
```c++
#include "openweathermap.h"
#include "HttpClient.h"


Weather* weather;
HttpClient* httpClient;

void setup() {		
	httpClient = new HttpClient();
	weather = new Weather("London,UK", httpClient,
			"INSERT your api key here!");
	weather->setCelsius();
	// or weather->setFahrenheit();
}

void loop() {
	weather_response_t resp = weather->cachedUpdate();
	if (resp.isSuccess) {
		Serial.print(resp.temp_low);
		Serial.print("-");
		Serial.print(resp.temp_high);
		Serial.println(resp.descr);
	}
}
```


There is also a non-cached method, if you want to handle caching yourself: 

```c++
		weather_response_t resp = weather->update();
```


Also see the example in the project directory. 


## Weather data
Currently the library loads the high and low temperature for today, including a short description. You can see the list of Condition-Codes at: http://openweathermap.org/wiki/API/Weather_Condition_Codes


### Can I extend it to include other data?
Sure! Look at the data: http://api.openweathermap.org/data/2.5/forecast/daily?q=London&mode=xml&units=metric&cnt=2

then change the method ``Weather::parse``, make sure the search keyword is unique. 


## How could I help? (TODO)
* test in Spark web ide
* make two distribution files (cpp and h) including everything, to make it easier to include in a project



Have fun! Share!