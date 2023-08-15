#include "DHT.h"

#define ALARM_PIN 23        // Buzzer pin
#define RELAY_FAN_PIN 19    // Fan relay pin
#define RELAY_HEATER_PIN 18 // Heater relay pin
#define DHTPIN 32           // DHT22 data pin
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

float minTemp; // Declare minTemp at a higher scope
float maxTemp; // Declare maxTemp at a higher scope

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(ALARM_PIN, OUTPUT);
  pinMode(RELAY_FAN_PIN, OUTPUT);
  pinMode(RELAY_HEATER_PIN, OUTPUT);
  digitalWrite(ALARM_PIN, LOW);      // Turn off the buzzer initially
  digitalWrite(RELAY_FAN_PIN, LOW); // Turn on the fan initially
  digitalWrite(RELAY_HEATER_PIN, HIGH); // Turn on the heater initially

  // Get desired output from the user
  Serial.println("Enter desired output (temperature range for heater control):");
  Serial.println("Min temperature (°C): ");
  while (!Serial.available()) {
    // Wait for user input
  }
  minTemp = Serial.parseFloat(); // Assign user input to minTemp
  Serial.println(minTemp);

  Serial.println("Max temperature (°C): ");
  while (!Serial.available()) {
    // Wait for user input
  }
  maxTemp = Serial.parseFloat(); // Assign user input to maxTemp
  Serial.println(maxTemp);
}

void loop() {
  // Read temperature and humidity from DHT22 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if the readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read data from DHT sensor!");
    return;
  }

  // Calculate the heat index
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

  // Print temperature, humidity, and heat index values
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\tHumidity: ");
  Serial.print(humidity);
  Serial.print(" %\tHeat Index: ");
  Serial.print(heatIndex);
  Serial.println(" °C");

  // Control the fan, heater, and buzzer based on temperature and user input
 
    if (temperature < minTemp) {
      digitalWrite(RELAY_HEATER_PIN, HIGH);   // Turn on the heater
      digitalWrite(RELAY_FAN_PIN, LOW);  
      if (temperature = minTemp+2) {
         digitalWrite(RELAY_HEATER_PIN, LOW);   // Turn on the heater
      digitalWrite(RELAY_FAN_PIN, LOW); 
      }

    } 
      digitalWrite(RELAY_HEATER_PIN, LOW);    // Turn off the heater
      digitalWrite(RELAY_FAN_PIN, LOW);  
    }
    
  
    
