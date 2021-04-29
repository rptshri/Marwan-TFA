// Client - COM25

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;

#include <Arduino_JSON.h>

// Replace with your network credentials
const char *ssid = "NodeMCU";      // Enter SSID here
const char *password = "12345678"; //Enter Password here

//Your IP address or domain name with URL path
const char *serverLogin = "http://192.168.1.1/login";
const char *serverPostData = "http://192.168.1.1/data";

String temperature;
int ResponseCode = 0;
char MyToken[] = "wfsnmyto-n47b-6bhc-7fam-4ltzatyixcmyg5";
String LastApiKey = "";

unsigned long previousMillis = 0;
const long interval = 20000;

bool isLoggedInFlag = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println();
  delay(30);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  while ((WiFiMulti.run() == WL_CONNECTED))
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  isLoggedInFlag = 0;
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    Serial.println();
    Serial.println("*******Inside Loop*******");
    if ((WiFiMulti.run() == WL_CONNECTED))
    {
      if (!isLoggedInFlag)
        doLogin();
      else
        sendData();

      previousMillis = currentMillis;
    }
    else
    {
      Serial.println("WiFi Disconnected");
      WiFiMulti.addAP(ssid, password);
      while ((WiFiMulti.run() == WL_CONNECTED))
      {
        delay(500);
        Serial.print(".");
      }
    }
  }
}

void doLogin()
{
  int flag = 0;
  ResponseCode = 0;
  while (flag < 10)
  {
    Serial.println("Trying to login!");
    ResponseCode = httpPOSTLoginRequest(serverLogin);
    if (ResponseCode == 202)
    {
      isLoggedInFlag = 1;
      flag = 11;
    }
    Serial.println(responseCodeToString(ResponseCode));
    flag++;
    delay(100);
  }
}

void sendData()
{
  int flag = 0;
  ResponseCode = 0;
  while (flag < 10)
  {
    Serial.println("Sending Data!");
    ResponseCode = httpPOSTSendData(serverPostData);
    if (ResponseCode == 201 || ResponseCode == 407)
    {
      flag = 11;
    }
    Serial.println(responseCodeToString(ResponseCode));
    flag++;
    delay(100);
  }
}

int httpPOSTSendData(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");

  JSONVar myObject;
  myObject["ApiKey"] = LastApiKey;
  myObject["data"] = String(random(18.00, 32.00));
  String jsonString = JSON.stringify(myObject);

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonString);

  String payload = "--";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);
    if (httpResponseCode == 407)
      isLoggedInFlag = 0;
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

int httpPOSTLoginRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");

  JSONVar myObject;
  myObject["username"] = "admin";
  myObject["password"] = "password123";
  myObject["Token"] = MyToken;
  String jsonString = JSON.stringify(myObject);

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonString);

  String payload = "--";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);
    JSONVar incomingMessage = JSON.parse(payload);
    payload = JSON.stringify(incomingMessage["ApiKey"]);
    payload.remove(0, 1);
    payload.remove(38, 1);
    LastApiKey = payload;
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

String responseCodeToString(int code)
{
  switch (code)
  {
    case 100:
      return F("Continue");
    case 101:
      return F("Switching Protocols");
    case 200:
      return F("OK");
    case 201:
      return F("Created");
    case 202:
      return F("Accepted");
    case 203:
      return F("Non-Authoritative Information");
    case 204:
      return F("No Content");
    case 205:
      return F("Reset Content");
    case 206:
      return F("Partial Content");
    case 300:
      return F("Multiple Choices");
    case 301:
      return F("Moved Permanently");
    case 302:
      return F("Found");
    case 303:
      return F("See Other");
    case 304:
      return F("Not Modified");
    case 305:
      return F("Use Proxy");
    case 307:
      return F("Temporary Redirect");
    case 400:
      return F("Bad Request");
    case 401:
      return F("Unauthorized");
    case 402:
      return F("Payment Required");
    case 403:
      return F("Forbidden");
    case 404:
      return F("Not Found");
    case 405:
      return F("Method Not Allowed");
    case 406:
      return F("Not Acceptable");
    case 407:
      return F("Proxy Authentication Required");
    case 408:
      return F("Request Time-out");
    case 409:
      return F("Conflict");
    case 410:
      return F("Gone");
    case 411:
      return F("Length Required");
    case 412:
      return F("Precondition Failed");
    case 413:
      return F("Request Entity Too Large");
    case 414:
      return F("Request-URI Too Large");
    case 415:
      return F("Unsupported Media Type");
    case 416:
      return F("Requested range not satisfiable");
    case 417:
      return F("Expectation Failed");
    case 500:
      return F("Internal Server Error");
    case 501:
      return F("Not Implemented");
    case 502:
      return F("Bad Gateway");
    case 503:
      return F("Service Unavailable");
    case 504:
      return F("Gateway Time-out");
    case 505:
      return F("HTTP Version not supported");
    default:
      return "Invalid Response Code";
  }
}
