// https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
// play with MDNS at last

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h> // Include the WebServer library

#include <Arduino_JSON.h>

char ClientToken[] = "wfsnmyto-n47b-6bhc-7fam-4ltzatyixcmyg5";
String LastApiKey = "";

/* Put your SSID & Password */
const char *ssid = "NodeMCU";      // Enter SSID here
const char *password = "12345678"; //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

unsigned long previousKeyCreationTime = 0;
const unsigned int KeyValidityInterval = 120000;

void setup()
{
  Serial.begin(9600);
  Serial.println("\n\n");

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP()); // Send the IP address of the ESP8266 to the computer

  server.on("/data", HTTP_POST, handle_OnData);
  server.on("/login", HTTP_POST, handle_Login);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  //  generateRandomKey();
}
void loop()
{
  server.handleClient();
  MDNS.update();
}

void handle_Login()
{ // If a POST request is made to URI /login
  Serial.println();
  Serial.println("*******Login Request!*******");

  if (server.hasArg("plain") == false)
  { //Check if body received
    Serial.println("No Body Received");
    JSONVar myObject;
    myObject["Error"] = "No Body Received";
    String jsonString = JSON.stringify(myObject);
    server.send(400, "application/json", jsonString);
    return;
  }
  String message = "Body received: ";
  message += server.arg("plain");
  Serial.println(message);

  JSONVar incomingMessage = JSON.parse(server.arg("plain"));
  if (JSON.typeof(incomingMessage) == "undefined")
  {
    Serial.println("Parsing input failed!");
    JSONVar myObject;
    myObject["Error"] = "Not Acceptable";
    String jsonString = JSON.stringify(myObject);
    server.send(406, "application/json", jsonString); // The request is invalid, so send HTTP status 400
    return;
  }
  if (!incomingMessage.hasOwnProperty("username") || !incomingMessage.hasOwnProperty("password") || incomingMessage["username"] == NULL || incomingMessage["password"] == NULL)
  {
    Serial.println("Invalid Key & Values!");
    JSONVar myObject;
    myObject["Error"] = "Non-Authoritative Information";
    String jsonString = JSON.stringify(myObject);
    server.send(203, "application/json", jsonString); // The request is invalid, so send HTTP status 400
    return;
  }
  if (JSON.stringify(incomingMessage["username"]) == "\"admin\"" && JSON.stringify(incomingMessage["password"]) == "\"password123\"")
  { // If both the username and the password are correct
    String Token = JSON.stringify(incomingMessage["Token"]);
    Token.remove(0, 1);
    Token.remove(38, 1);
    if (Token == ClientToken)
    {
      Serial.println("Initial Token validated | Sending API Key");
      JSONVar myObject;
      LastApiKey = generateRandomKey();
      myObject["ApiKey"] = LastApiKey;
      String jsonString = JSON.stringify(myObject);
      server.send(202, "application/json", jsonString);
      previousKeyCreationTime = millis(); //save the time of Api Creation
      Serial.print("previousKeyCreationTime:  ");
      Serial.println(previousKeyCreationTime);
    }
    else
    {
      JSONVar myObject;
      myObject["Status"] = "Accepted but wrong Token Key";
      String jsonString = JSON.stringify(myObject);
      server.send(202, "application/json", jsonString);
    }
  }
  else
  {
    JSONVar myObject;
    myObject["Error"] = "Bad Request";
    String jsonString = JSON.stringify(myObject); // Username and password don't match
    server.send(400, "application/json", jsonString);
  }
}

void handle_OnData()
{
  Serial.println();
  Serial.println("*******Data Receive!*******");
  Serial.print("Last ApiKey Valid till:  ");
  int timeRemaining = KeyValidityInterval - (millis() - previousKeyCreationTime);
  Serial.println(timeRemaining);

  if (timeRemaining < 0)
  {
    JSONVar myObject;
    myObject["Status"] = "Proxy Authentication Required";
    String jsonString = JSON.stringify(myObject);
    server.send(407, "application/json", jsonString);
  }

  if (server.hasArg("plain") == false)
  { //Check if body received
    Serial.println("No Body Received");
    JSONVar myObject;
    myObject["Error"] = "No Body Received";
    String jsonString = JSON.stringify(myObject);
    server.send(400, "application/json", jsonString);
    return;
  }
  String message = "Body received: ";
  message += server.arg("plain");
  Serial.println(message);

  JSONVar incomingMessage = JSON.parse(server.arg("plain"));
  if (!incomingMessage.hasOwnProperty("ApiKey") || !incomingMessage.hasOwnProperty("data") || incomingMessage["ApiKey"] == NULL || incomingMessage["data"] == NULL)
  {
    Serial.println("Invalid Key & Values!");
    JSONVar myObject;
    myObject["Error"] = "Non-Authoritative Information";
    String jsonString = JSON.stringify(myObject);
    server.send(203, "application/json", jsonString); // The request is invalid, so send HTTP status 400
    return;
  }

  String ClientApiKey = JSON.stringify(incomingMessage["ApiKey"]);
  ClientApiKey.remove(0, 1);
  ClientApiKey.remove(38, 1);
  if (ClientApiKey == LastApiKey)
  {
    Serial.println("ApiKey validated | Saving your data!");
    JSONVar myObject;
    myObject["Status"] = "Data Received!";
    String jsonString = JSON.stringify(myObject);
    server.send(201, "application/json", jsonString);
  }
  else
  {
    Serial.println("ApiKey validated | Saving your data!");
    JSONVar myObject;
    myObject["Status"] = "Non-Authoritative Information!";
    String jsonString = JSON.stringify(myObject);
    server.send(203, "application/json", jsonString);
  }
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

String RandomKey = "";
String generateRandomKey()
{
  // dc52bc78-8011-47e6-ab1b-5b29ca1aaeee
  RandomKey = "";
  char msg[40];
  for (int i = 0; i < 38; i++)
  {
    if (i == 8 || i == 13 || i == 18 || i == 23)
      RandomKey += '-';
    else
    {
      char randomValue = random(0, 35);

      if (randomValue > 25)
      {
        RandomKey += char((randomValue - 26) + '0');
      }
      else
        RandomKey += char(randomValue + 'a');
    }
  }
  Serial.print("Here is your random string: ");
  Serial.println(RandomKey);

  return RandomKey;
}
