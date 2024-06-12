/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <milliswaith.h>

// Update these with values suitable for your network.

const char* ssid = "watchdog";
const char* password = "bl@ckd0g";
const char* mqtt_server = "192.168.1.163";
const char* intopic = "switch2";
const char* outtopic = "state2";
const char* signalS = "wifiS2";
const int CTRpin = 0;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
String stateS = "";

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(const char* topic, byte* payload,  int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String messegeS = "";
  for (int i = 0; i <  length; i++) {
    Serial.print((char)payload[i]);
    messegeS += (char)payload[i];
  }
  Serial.println("HHH");
  Serial.println("messegeS=");
  Serial.println(messegeS);

  // Switch on the LED if an 1 was received as first character
  if (String(topic) == String(intopic) ) {
    Serial.println("topic check!");

    if (messegeS == String("On") ) {
    digitalWrite(CTRpin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    stateS = "LED ON";
    client.publish(outtopic, "LED ON");
    }
    else if (messegeS == String("Off")) {
    digitalWrite(CTRpin, HIGH);  // Turn the LED off by making the voltage HIGH
    stateS = "LED OFF";
    client.publish(outtopic, "LED OFF");
    }    
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outtopic, "hello world");
      // ... and resubscribe
      client.subscribe(intopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
  delay(3000);
  Serial.println("Looping");  
}

void setup() {
  pinMode(CTRpin, OUTPUT);     // Initialize the CTRpin pin as an output
  digitalWrite(CTRpin, HIGH);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  unsigned long now = millis();
  if (now - lastMsg > 60000) {
    int wifiStrenghtI = 0;
    wifiStrenghtI = WiFi.RSSI();

    //client.publish(signalS, String(wifiStrenghtI));
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "Wifi Signal :%d", wifiStrenghtI);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    client.publish(signalS, msg);
    Serial.print("stateS=");
    Serial.println(stateS);
    
  }
  
}
