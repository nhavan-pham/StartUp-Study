#include <ArduinoWebsockets.h>
#include "esp_http_server.h"
#include <WiFi.h>


using namespace std;


// using for get weather
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=Ho Chi Minh,VN&APPID=";
const String key = "ce2251bbc35d893abb8245394703e1e5";
int cycleTime = 1000;




/*
 * Use for get the weather infor from openweathermap.org
 * Return type : String
 * Ex : {"coord":{"lon":106.6667,"lat":10.75},"weather":[{"id":802,"main":"Clouds","description":"scattered clouds","icon":"03d"}],"base":"stations","main":{"temp":306.12,"feels_like":314.86,"temp_min":306.12,"temp_max":306.12,"pressure":1010,"humidity":66},"visibility":7000,"wind":{"speed":2.57,"deg":350},"clouds":{"all":40},"dt":1620448156,"sys":{"type":1,"id":9314,"country":"VN","sunrise":1620426775,"sunset":1620472004},"timezone":25200,"id":1566083,"name":"Ho Chi Minh City","cod":200}
 */
/*
* function :
*/

String getWeather()
{
    HTTPClient http;
    String payload = "";
 
    http.begin(endpoint + key); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        payload = http.getString();
        Serial.println(httpCode);
      }
    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
    return payload;
}

//
void collectWeatheByCycle()
{

}

// setup cy