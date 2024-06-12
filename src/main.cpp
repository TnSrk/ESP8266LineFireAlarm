/*
ESP01 pubsub 2 Relays

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
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <secret.h>
#include <Adafruit_AHTX0.h>
#include <sstream>
#include <vector>
#include <Wire.h>
#include <timemaker.h>
#include <secureLoad.cpp>



//#include <milliswaith.h>

// Update these with values suitable for your network.

std::string macS = "FF:FF:FF:FF:FF:FF";
//const char* intopic = "switch2";
std::string tp = "mac/" + macS ;
std::string intopic = tp + "/cmd" ;
std::string outtopic = tp +  "/state";
//const char* outtopic = "state2";
//std::string outtopic = macS +  "/state";
//const char* subtopic = "FrontSleeptime";
//std::string subtopic = macS +  "/reportInterval";
//std::string macS ;
//std::string weatherTopic = "weather";
//std::string subtopic = macS +  "/weather";

//const char* intopicb = "switch2b";
//const char* outtopicb = "state2b";
//const char* signalS = "wifiS2";

const int AnalogReadPin = A0;
const int MQ_Dpin = 12; //D6
const int MQ_AlarmPin = 13; //D7
int alarm_flag_I = 2;
const int CTRpin = 0; 
const int CTRpinb = 2;
bool blink_b = 0;
bool blink_state_b = 1;
const int blink_pin = 14; //D5
unsigned long prev_blink_msU = millis();
unsigned long blink_interval = 500;
//const int CTRpin = 0;
//const int CTRpinb = 2;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = millis();
unsigned long alarmCountDown = millis();
unsigned long alarmCheckInterval = 30 * 1000; // Checking interval
bool stateHold_b = 1;
int value = 0;
//String stateS = "";
unsigned long interval = 1000 * 30;
unsigned long previousMillis = millis();

//Wire Wire_ptr;

//Temp & moist parameters
//Adafruit_AHTX0 aht;

secureLoad_CL CL;

float averageF(std::vector<float> vF ){
  int n = vF.size();

  float Vout0 = 0.0;
  for (int i = 0; i < n; i += 1){
    Vout0 += vF[i];
    //bool b = milliswait2(10);
    //delay(10);
  }

  float Vout = Vout0/float(n);
  // exclude outlier
  int nn = 0;
  float v = (Vout/10.0)*2;
  float Vout0b = 0;
  float vRead = 0;
  
  for (int i = 0; i < n; i += 1){
    //vRead = analogRead(vreadPin);
    if ( ((vF[i] - Vout) < v) ||  ((Vout - vF[i]) < v) ) {
      nn +=1;
      Vout0b += vF[i];
    }
    //bool b = milliswait2(10);
    //delay(10);
  }
  Vout = Vout0b/float(nn);

  return Vout;
}

class ahtReport {
  
  public:
    Adafruit_AHTX0 aht;
    float tVF ;
    float hVF ;
    uint MQ_state_b = 1;
    
    //Wire.begin(0,2);
    //void begin(){
    //  Wire.begin(0,2);
    //}

    std::string weather_report(int sampleI){  
      //begin();

      std::string message = "t=100.1:h=100.1";            
      std::vector<float> hV;
      std::vector<float> tV;
      
      
      if (! aht.begin(&Wire)) {
        //Serial.println("Could not find AHT? Check wiring");
        //while (1) delay(10);
      }

      else {
      //Serial.println("AHT10 or AHT20 found");
      

      for (int i=0;i <= sampleI; i += 1) {
        sensors_event_t humidity, temp;
        while (!aht.getEvent(&humidity, &temp)) {// populate temp and humidity objects with fresh data
        //Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
        //Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
          delay(10);
        }
        //if ( temp.temperature > 0 ){}; 
        tV.push_back(temp.temperature);
        //float tempF = aht.getTemperatureSensor();
        hV.push_back(humidity.relative_humidity);
          
        }
        delay(100);
      //char buffer[50];
      }

      //Wire.endTransmission();

       tVF = averageF(tV);
       hVF = averageF(hV);
       MQ_state_b = digitalRead(MQ_Dpin);

      message = std::string("t=");
      message += std::to_string(tVF);
      message += std::string(":h=");
      message += std::to_string(hVF); 
      message += std::string(":mq=");
      message += std::to_string(MQ_state_b);
      //client.publish(weatherTopic, message);
           
      return message;
  }


  void report(){
      int wifiStrenghtI = 0;
      wifiStrenghtI = WiFi.RSSI();
      //Serial.print("Publish message: ");
      //Serial.println(msg);
      std::string signalS = tp+"/signal";
      client.publish(signalS.c_str(), std::to_string(wifiStrenghtI).c_str());

      std::string weatherTopic = tp + "/weather";
      std::string message_str = std::string ("macS:" );
      message_str += macS;
      message_str += ":";
      message_str += weather_report(10);
      client.publish(weatherTopic.c_str(), message_str.c_str());

  }

};

float mqRead(int sampleI){
  std::string message = "MQ-Read=";
  std::vector<float> mqV;
  for (int i=0;i <= sampleI; i += 1) {
        float mqr = analogRead(AnalogReadPin);
        mqV.push_back( mqr );                      
        delay(100);
      }
  float mqVF = averageF(mqV);
  return mqVF;
}



void setup_wifi() {

  //delay(10);
  // We start by connecting to a WiFi network
  /*
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        //Serial.print(".");
        delay(1000);
  }
  
  /*
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  */
  
}

ahtReport W_report;
/*
void alarmVoice(uint timeU){
  pinMode(MQ_AlarmPin, OUTPUT);
  
  unsigned long prevTime = millis();
  unsigned long intervalW = 1000;
  uint i = 0;
  
  bool pmB = HIGH;
  while (i < timeU) {
    unsigned long currTime = millis();
      if (currTime - prevTime >= intervalW ) {
        i += 1;
        
        digitalWrite(MQ_AlarmPin, !pmB);
        pmB = !pmB;
        prevTime = currTime;
    }

  }
  digitalWrite(MQ_AlarmPin, HIGH);

}
*/

bool hotAlarm(){
  bool b = 0;
  bool mqValB = digitalRead(MQ_Dpin);
  W_report.weather_report(10);
  float tempF = W_report.tVF;
  std::string message = "Normal";

  if (!mqValB || (tempF > 45.0) ) {
    alarm_flag_I = 0;
    message = "Alert!\n\r";
    message += "Temperature = ";
    message += std::to_string(tempF);
    message += "\n\r";
    alarm_flag_I = 0;
  }
  else {
    alarm_flag_I = 1;
  }
  if (!mqValB ) {
    
    message += "\n\rDanger Factor Detected !!! \n\r";    
    //
  }
  message += macS;
  message += "\n\r";
  message += localTime();
  

  if (stateHold_b != alarm_flag_I && alarm_flag_I  == 0) { 
    //digitalWrite(MQ_AlarmPin,LOW);    
    blink_b = 1;
    CL.lineSend(message);      
  }

  else if (stateHold_b != alarm_flag_I && alarm_flag_I  == 1) {    
    //CL.lineSend(message);  
    blink_b = 0;
    //digitalWrite(MQ_AlarmPin,HIGH);    
  }

  stateHold_b = alarm_flag_I;
  
  Serial.println(message.c_str());
  
  return b;

}

void callback(const char* topic, byte* payload,  int length) {
  /*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  */
  //std::string in_topic_str = "mac/" + macS + "/cmd";
  std::string messegeS = "";
  for (int i = 0; i <  length; i++) {
    //Serial.print((char)payload[i]);
    messegeS += (char)payload[i];
  }
  /*
  Serial.println("HHH");
  Serial.println("messegeS=");
  Serial.println(messegeS);
  */
  // Switch on the LED if an 1 was received as first character
  //Serial.println("topic callback");
  //Serial.println(topic);
  //Serial.println("intopic_str callback");
  std::string intopic_str = "mac/" + macS + "/cmd";
  //Serial.println(intopic_str.c_str());

  if  ( std::string(topic) ==  "mac/" + macS + "/cmd" ) {
    //Serial.println("topic check!");
    std::string stateS = "";
    if (messegeS == std::string("On") ) {
      digitalWrite(CTRpin, LOW);   // Turn the LED on (Note that LOW is the voltage level
      //stateS = "LED ON";
      //snprintf (stateS, MSG_BUFFER_SIZE, "%s",  "LED ON");
      stateS =  "LED ON";
      
      client.publish(outtopic.c_str(), stateS.c_str());
    }
    else if (messegeS == std::string("OnB") ) {
      digitalWrite(CTRpinb, LOW);   // Turn the LED on (Note that LOW is the voltage level
      //stateS = "LED ON";
      //snprintf (stateS, MSG_BUFFER_SIZE, "%s",  "LED ON");
      stateS =  "LED B ON";
      
      client.publish(outtopic.c_str(), stateS.c_str());
    }
    else if (messegeS == std::string("Off")) {
      digitalWrite(CTRpin, HIGH);  // Turn the LED off by making the voltage HIGH
      //stateS = "LED OFF";
      //snprintf (stateS, MSG_BUFFER_SIZE, "%s", "LED OFF");
      stateS =  "LED OFF";
      client.publish(outtopic.c_str(), stateS.c_str());
    } 
    else if (messegeS == std::string("OffB")) {
      digitalWrite(CTRpinb, HIGH);  // Turn the LED off by making the voltage HIGH
      //stateS = "LED OFF";
      //snprintf (stateS, MSG_BUFFER_SIZE, "%s", "LED OFF");
      stateS =  "LED B OFF";
      client.publish(outtopic.c_str(), stateS.c_str());
    }

  }   

  else if (std::string(topic) ==  "mac/" + macS + "/reportInterval" ) {
    //interval = (unsigned long)messegeS.toInt();
    interval = ( unsigned long ) std::stoi(messegeS) * 1000UL;
  }

  /*
  else if (String(topic) == String(intopicb) ) {
    Serial.println("topic check!");

    if (messegeS == String("On") ) {
    digitalWrite(CTRpinb, LOW);   // Turn the LED on (Note that LOW is the voltage level
    //stateSb = "LED ON";
    snprintf (stateSb, MSG_BUFFER_SIZE, "%s", "LED ON");
    client.publish(outtopicb,  stateSb);
    }

    else if (messegeS == String("Off")) {
    digitalWrite(CTRpinb, HIGH);  // Turn the LED off by making the voltage HIGH
    //stateSb = "LED OFF";
    snprintf (stateSb, MSG_BUFFER_SIZE, "%s", "LED OFF");
    client.publish(outtopicb, stateSb);
    }    
  }
  */

}

void reconnect() {
  //Serial.println("RC1");
  // Loop until we're reconnected
  //std::string intopic = macS ;
  //std::string outtopic = macS + "/state";
  while (!client.connected()) {
    //Serial.println("RC2");
    //Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    //std::string clientId = "ESP8266Client-";
    //String clientId += String(random(0xffff), HEX);
    // Attempt to connect
    std::string clientId = "ESP01";
    clientId += macS; // Create unique client ID based on mac address

    if (client.connect(clientId.c_str())) {
      //Serial.println("RC3");
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outtopic.c_str(), "hello world");
      //client.publish(outtopicb, "hello world");
      // ... and resubscribe
      std::string itp = tp + "/#";
      //Serial.println("itp");
      //Serial.println(itp.c_str());
      client.subscribe(itp.c_str());
      //client.subscribe(subtopic );
      //client.subscribe(intopicb);
    } else {
      /*
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      */
      delay(500);
    }
  }
  delay(500);
  //Serial.println("Looping");  
}



void setup() {
  //Wire.begin(0,2);
  //W_report.begin();
  //macSTR.remove(20);
  pinMode(CTRpin, OUTPUT);     // Initialize the CTRpin pin as an output
  digitalWrite(CTRpin, HIGH);
  pinMode(CTRpinb, OUTPUT);     
  digitalWrite(CTRpinb, HIGH);

  pinMode(blink_pin, OUTPUT); 
  digitalWrite(blink_pin, HIGH);
  
  /*
  pinMode(CTRpinb, OUTPUT);     // Initialize the CTRpin pin as an output
  digitalWrite(CTRpinb, HIGH);
  */
  Serial.begin(115200);  
  //Serial.println("WiFi.macAddress().c_str()");
  macS = std::string(WiFi.macAddress().c_str());
  //Serial.println(macS.c_str());

  setup_wifi();
  timeMaker();
  CL.begin();
  
  tp = "mac/";
  //Serial.println("1");

  tp += macS.c_str() ;
  //Serial.println("2");
  intopic = tp + "/cmd" ;
  //Serial.println("intopic");
  //Serial.println(intopic.c_str());
  outtopic = tp +  "/state";
  //Serial.println("4");

  //outtopic "mac/" + macS + "/state";
  
  //intopic = macS ;
  //outtopic = macS +  "/state";
  //subtopic = macS +  "/reportInterval";

  client.setServer(mqtt_server, 1883);
  //Serial.println("5");
  client.setCallback(callback);
  //Serial.println("6");
  //weatherTopic += ":" + macS ;
  //W_report.report();  
  pinMode(MQ_Dpin, INPUT_PULLUP);
  pinMode(MQ_AlarmPin, OUTPUT);
  digitalWrite(MQ_AlarmPin, HIGH);
}

void loop() {

  unsigned long currentMillis = millis();
  
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    //Serial.print(millis());
    //Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    //Serial.println(WiFi.localIP());
    //Alternatively, you can restart your board
    //ESP.restart();
    //Serial.println(WiFi.RSSI());
    previousMillis = currentMillis;
  }

  if ( (WiFi.status() == WL_CONNECTED) && !client.connected() ) {
    timeMaker();
    reconnect();
    Serial.println("reconnect()");
  }
  else if (WiFi.status() == WL_CONNECTED && client.connected()) {
    client.loop();
  }
  
  unsigned long now = millis();
  if (client.connected() && now - lastMsg >= interval) {
    lastMsg = now;
    W_report.report();        
  }

  if (now - alarmCountDown >= alarmCheckInterval) {
    alarmCountDown = now;
    if (hotAlarm()){
      Serial.println("hotAlarm Called");
    }
  }

  if (blink_b){
    if (now - prev_blink_msU >= blink_interval ) {
      blink_state_b = !blink_state_b;
      digitalWrite(blink_pin,blink_state_b);
      digitalWrite(MQ_AlarmPin,blink_state_b);    
      prev_blink_msU = millis();
    }
    
  }
  else {
    blink_state_b = 1;
    digitalWrite(blink_pin,blink_state_b);
    digitalWrite(MQ_AlarmPin,blink_state_b);  
  }
  
  if (now - lastMsg >= interval*2000) {
    lastMsg = now;
    ESP.restart();    
  }
  
}
