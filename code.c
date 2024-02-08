#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "MEO-620B4B";
const char *password = "ff@700786";

const char *mqtt_server = "192.168.2.14";
const int mqtt_port = 1883;
const char *mqtt_user = "ad001";
const char *mqtt_password = "987pok@01";

const char *deviceID = "334343200EA00610";
const char *controlTopic = "home/automation/control";

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
  Serial.print("Message arrived");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  if (message.startsWith(deviceID)) {
    String command = message.substring(message.indexOf(":") + 1);
    if (command.equals("fan-on")) {
      digitalWrite(fanRelayPin, HIGH);
    } else if (command.equals("fan-off")) {
      digitalWrite(fanRelayPin, LOW); 
    } else if (command.equals("tubelight-on")) {
      digitalWrite(tubelightRelayPin, HIGH); 
    } else if (command.equals("tubelight-off")) {
      digitalWrite(tubelightRelayPin, LOW); 
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(deviceID, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(controlTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
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
