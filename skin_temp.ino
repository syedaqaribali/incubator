#include "DHT.h"
#include <Adafruit_MAX31865.h>

#define ALARM_PIN 23        // Buzzer pin
#define RELAY_FAN_PIN 19    // Fan relay pin
#define RELAY_HEATER_PIN 18 // Heater relay pin
#define DHTPIN 32           // DHT22 data pin
#define DHTTYPE DHT22 
// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

DHT dht(DHTPIN, DHTTYPE);

float minTemp; // Declare minTemp at a higher scope
float maxTemp; // Declare maxTemp at a higher scope

void setup() {
  Serial.begin(115200);
  dht.begin();
  thermo.begin(MAX31865_3WIRE); 

  pinMode(ALARM_PIN, OUTPUT);
  pinMode(RELAY_FAN_PIN, OUTPUT);
  pinMode(RELAY_HEATER_PIN, OUTPUT);
  digitalWrite(ALARM_PIN, LOW);      // Turn off the buzzer initially
  digitalWrite(RELAY_FAN_PIN, HIGH); // Turn on the fan initially
  digitalWrite(RELAY_HEATER_PIN, HIGH); // Turn on the heater initially

  // Get desired output from the user
  Serial.println("Enter desired output (temperature range for heater control):");
  Serial.print("Min temperature (°C): ");
  while (!Serial.available()) {
    // Wait for user input
  }
  minTemp = Serial.parseFloat(); // Assign user input to minTemp
  Serial.println(minTemp);

  Serial.print("Max temperature (°C): ");
  while (!Serial.available()) {
    // Wait for user input
  }
  maxTemp = Serial.parseFloat(); // Assign user input to maxTemp
  Serial.println(maxTemp);
}

void loop() {
    uint16_t rtd = thermo.readRTD();

  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 4095;
  Serial.print("Ratio = "); Serial.println(ratio,8);
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  Serial.print("Temperature = "); Serial.println(thermo.temperature(RNOMINAL, RREF));

  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
  
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
  if (heatIndex >= 39.0) {
    digitalWrite(RELAY_FAN_PIN, LOW);       // Turn off the fan
    digitalWrite(RELAY_HEATER_PIN, LOW);    // Turn off the heater
    digitalWrite(ALARM_PIN, HIGH);          // Turn on the buzzer
  } else {
    if (temperature < minTemp ) {
      digitalWrite(RELAY_HEATER_PIN, HIGH);   // Turn on the heater
      digitalWrite(RELAY_FAN_PIN, LOW); 
       if (temperature = maxTemp+1 ) {
      digitalWrite(RELAY_HEATER_PIN, HIGH);   // Turn on the heater
      digitalWrite(RELAY_FAN_PIN, LOW);  
       }
    } else {
      digitalWrite(RELAY_HEATER_PIN, LOW);    // Turn off the heater
      digitalWrite(RELAY_FAN_PIN, LOW);  
    }
    digitalWrite(RELAY_FAN_PIN, LOW);      // Turn on the fan
    digitalWrite(ALARM_PIN, LOW);           // Turn off the buzzer
  }

  
}
