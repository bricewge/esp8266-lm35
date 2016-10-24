/* WeMos DHT Server
 *
 * Connect to WiFi and respond to http requests with temperature and humidity
 *
 * Based on Adafruit ESP8266 Temperature / Humidity Webserver
 * https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver
 *
 * Depends on Adafruit DHT Arduino library
 * https://github.com/adafruit/DHT-sensor-library
 */

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define LM35PIN 0

// Existing WiFi network
const char* ssid     = "Mycorhize";
const char* password = "wagnirdloffr";

// Listen for HTTP requests on standard port 80
ESP8266WebServer server(80);

float humidity, temperature;                 // Raw float values from the sensor
char str_humidity[10], str_temperature[10];  // Rounded sensor values and as strings
// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;            // When the sensor was last read
const long interval = 2000;                  // Wait this long until reading again

void handle_root() {
  server.send(200, "text/plain", "TEST DHT Server. Get /temp");
  delay(100);
}

void read_sensor() {
  int lm35Value;
  
  lm35Value = analogRead(LM35PIN);
  temperature = map(lm35Value, 0, 1023, 0, 255);
  // Convert the floats to strings and round to 2 decimal places
  dtostrf(temperature, 1, 2, str_temperature);
  
  Serial.print("Temperature: ");
  Serial.print(str_temperature);
  Serial.println(" °C");
}

void json_result() {
    read_sensor();
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["temperature"] = temperature;
    json["unit"] = "celcius";
    char buffer[256];
    json.prettyPrintTo(buffer, sizeof(buffer));
    server.send(200, "text/plain", buffer);
}

void setup(void)
{
  // Open the Arduino IDE Serial Monitor to see what the code is doing
  Serial.begin(115200);

  Serial.println("WeMos LM35 Server");
  Serial.println("");

  // Connect to your WiFi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  // Wait for successful connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  // Initial read
  read_sensor();

  // Handle http requests
  server.on("/", json_result);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  // Listen for http requests
  server.handleClient();
}
