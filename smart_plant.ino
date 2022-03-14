/*
 * Smart Plant: aplicação para auxilio a rega de plantas com IoT.
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PUBLISH_INTERVAL  10000  // milliseconds

// Wi-Fi Config
const char* ssid     = "O nome da sua rede WiFi";
const char* password = "A senha da rede";

// Broker Config
const char* mqttBroker = "mqtt.tago.io";
const char* topic      = "tago/data/post";
const char* clientId   = "ESP8266-Test";
const char* authMeth   = "use-token-auth";
const char* token      = "Seu Token criado na aplicação na Tago.io";

// Message
const char* variable = "umidade";
int value = 100;
char msg[100];

// Soil moisture sensor
const int analogSensorPin = A0;

WiFiClient nodeMCUClient;
PubSubClient client(nodeMCUClient);
long lastMsg = 0;

void setup(void)
{
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  randomSeed(20);
  setup_wifi();
  client.setServer(mqttBroker, 1883);
}


void setup_wifi(void)
{
  delay(10);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void reconnect(void)
{
  // Loop until we're reconnected
  while(!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if(client.connect(clientId, authMeth, token))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop(void)
{
  if(!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if(now - lastMsg > PUBLISH_INTERVAL)
  {
    lastMsg = now;
    
    // Get soil moisture value
    int sensorValue = analogRead(analogSensorPin);
    value = map(sensorValue, 1000, 0, 0, 1023);

    // Assemble message
    sprintf(msg, "{\"variable\":\"%s\", \"value\":%d}", variable, value);
    
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic, msg);
  }
}
