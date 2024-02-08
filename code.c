#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
const char *ssid = "DuplicateWiFiSSID";
const char *password = "DuplicateWiFiPassword";

// Replace with your MQTT broker details
const char *mqtt_server = "duplicate-MQTT-broker-IP";
const int mqtt_port = 1883;
const char *mqtt_user = "duplicate-MQTT-username";
const char *mqtt_password = "duplicate-MQTT-password";

// Replace with your device ID and topic
const char *deviceID = "duplicate-device-ID";
const char *controlTopic = "home/automation/control";

// Pins connected to relays for controlling appliances
const int fanRelayPin = D1;
const int tubelightRelayPin = D2;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the received message is for this device
  if (message.startsWith(deviceID)) {
    // Extract the control command from the message
    String command = message.substring(message.indexOf(":") + 1);

    // Perform action based on the command
    if (command.equals("fan-on")) {
      digitalWrite(fanRelayPin, HIGH); // Turn on the fan
    } else if (command.equals("fan-off")) {
      digitalWrite(fanRelayPin, LOW); // Turn off the fan
    } else if (command.equals("tubelight-on")) {
      digitalWrite(tubelightRelayPin, HIGH); // Turn on the tubelight
    } else if (command.equals("tubelight-off")) {
      digitalWrite(tubelightRelayPin, LOW); // Turn off the tubelight
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(deviceID, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      
      // Once connected, subscribe to the control topic
      client.subscribe(controlTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(fanRelayPin, OUTPUT);
  pinMode(tubelightRelayPin, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
