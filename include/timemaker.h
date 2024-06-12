#ifndef TIMEMAKER
#define TIMEMAKER
#include <Arduino.h>
#include <sstream>

//time_t now = time(nullptr);

void timeMaker(){
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  //tm *tm_local = localtime(&now);
  Serial.println("");
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
   
}

void printLocalTime(){
  time_t now;
  now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

std::string  localTime(){
  time_t now;
  now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  
  //Serial.print("Current time: ");
  return std::string( asctime(&timeinfo) );
}

struct tm localTimeST(){
  time_t now;
  now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);  
  return timeinfo;
}




#endif //TIMEMAKER