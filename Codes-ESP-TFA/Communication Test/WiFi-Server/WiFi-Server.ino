//https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
// play with MDNS at last

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h> // Include the WebServer library

#include <Arduino_JSON.h>

/* Put your SSID & Password */
const char *ssid = "NodeMCU";      // Enter SSID here
const char *password = "12345678"; //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

void setup()
{
  Serial.begin(9600);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP()); // Send the IP address of the ESP8266 to the computer

  server.on("/", HTTP_POST, handle_OnConnect);
  server.on("/login", HTTP_POST, handleLogin);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void loop()
{
  server.handleClient();
  MDNS.update();
}

void handle_OnConnect()
{
  Serial.println("Connection Request!");

  if (server.hasArg("plain") == false)
  { //Check if body received
    server.send(200, "text/plain", "Body not received");
    return;
  }
  String message = "Body received:\n";
  message += server.arg("plain");
  message += "\n";
  Serial.println(message);

  JSONVar incomingMessage = JSON.parse(server.arg("plain"));
  if (JSON.typeof(incomingMessage) == "undefined")
  {
    Serial.println("Parsing input failed!");
    return;
  }

  Serial.print("JSON.typeof(incomingMessage) = ");
  Serial.println(JSON.typeof(incomingMessage)); // prints: object

  if (incomingMessage.hasOwnProperty("hi"))
  {
    Serial.print("incomingMessage[\"hi\"] = ");
    Serial.println(incomingMessage["hi"]);
  }
  else
  {
    Serial.println("No hi");
  }

  JSONVar myObject;
  myObject["hello"] = "world";
  myObject["true"] = true;
  myObject["x"] = 42;
  String jsonString = JSON.stringify(myObject);
  server.send(200, "application/json", jsonString);
}

void handleLogin()
{ // If a POST request is made to URI /login

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
    myObject["Error"] = "Invalid Json";
    String jsonString = JSON.stringify(myObject);
    server.send(400, "application/json", jsonString); // The request is invalid, so send HTTP status 400

    return;
  }

  if (!incomingMessage.hasOwnProperty("username") || !incomingMessage.hasOwnProperty("password") || incomingMessage["username"] == NULL || incomingMessage["password"] == NULL)
  {
    Serial.println("Invalid Key & Values!");
    JSONVar myObject;
    myObject["Error"] = "Invalid key & Values";
    String jsonString = JSON.stringify(myObject);
    server.send(400, "application/json", jsonString); // The request is invalid, so send HTTP status 400
    return;
  }

  if (JSON.stringify(incomingMessage["username"]) == "\"admin\"" && JSON.stringify(incomingMessage["password"]) == "\"password123\"")
  { // If both the username and the password are correct
    JSONVar myObject;
    myObject["Status"] = "Logged In Successfully!";
    String jsonString = JSON.stringify(myObject);
    server.send(201, "application/json", jsonString);
  }
  else
  {
    JSONVar myObject;
    myObject["Error"] = "Unauthorized";
    String jsonString = JSON.stringify(myObject); // Username and password don't match
    server.send(401, "application/json", jsonString);
  }
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}
