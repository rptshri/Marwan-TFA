// Client - COM25

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;

#include <Arduino_JSON.h>

// Replace with your network credentials
const char* ssid = "NodeMCU";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

//Your IP address or domain name with URL path
const char *serverNameTemp = "http://192.168.4.1/temperature";
const char *serverPost = "http://192.168.1.1/";
const char *serverLogin = "http://192.168.1.1/login";

String temperature;
int LoginResponseCode;

unsigned long previousMillis = 0;
const long interval = 5000;

void setup()
{
  Serial.begin(9600);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  while ((WiFiMulti.run() == WL_CONNECTED))
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  if (WiFiMulti.run() == WL_CONNECTED)
    Serial.println("Connected to WiFi");
  else
    Serial.println("No WiFi");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    Serial.println((WiFiMulti.run() == WL_CONNECTED));
    // Check WiFi connection status
    if ((WiFiMulti.run() == WL_CONNECTED))
    {
      //      postMethod = httpPOSTRequest(serverPost);

      int counter = 0;
      while (LoginResponseCode == 201 || counter == 10) {
        Serial.println("Trying to login!");
        LoginResponseCode = httpPOSTRequest(serverLogin);
        if (LoginResponseCode == 400 || LoginResponseCode == 401)
        {
          Serial.println("Please Check Authorization credentials!");
          return;
        }
        counter++;
        delay(30);
      }

      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
  }
}

int httpPOSTRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);
  //  http.addHeader("Content-Type", "text/plain");
  http.addHeader("Content-Type", "application/json");
  //  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Token", "asdas-asdasd-adasd-asdasd-dasd");

  JSONVar myObject;
  myObject["username"] = "admin";
  myObject["password"] = "password123";
  myObject["Token"] = "asdas-asdasd-adasd-asdasd-dasd";
  // JSON.stringify(myVar) can be used to convert the json var to a String
  String jsonString = JSON.stringify(myObject);

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonString);

  String payload = "--";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return httpResponseCode;
}



String httpGETRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);

  // Send HTTP Get request
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
