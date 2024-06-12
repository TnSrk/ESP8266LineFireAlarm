#ifndef AVERAGEV
#define AVERAGEV
#include <Arduino.h>

int averageF(float vreadPin, int sampleI ){
  int n = sampleI;
  int Vout0 = 0;
  for (int i = 1; i <= n; i++){
    Vout0 += analogRead(vreadPin);
    //bool b = milliswait2(10);
    delay(10);
  }
  int Vout = Vout0/n;
  // exclude outlier
  int nn = 0;
  int v = (Vout/10)*2;
  int Vout0b = 0;
  int vRead = 0;
  
  while (nn <= sampleI) {
    vRead = analogRead(vreadPin);
    if ( ((vRead - Vout) < v) ||  ((Vout - vRead) < v) ) {
      nn +=1;
      Vout0b += vRead;
    }
    //bool b = milliswait2(10);
    delay(10);
  }
  Vout = Vout0b/nn;

  return Vout;
}

#endif //AVERAGEV