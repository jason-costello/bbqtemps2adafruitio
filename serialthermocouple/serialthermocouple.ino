/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "config.h"

//AdafruitIO_Group *grp = io.group("bbq");
AdafruitIO_Feed *curTemp = io.feed("bbq.curTemp");
AdafruitIO_Feed *prevTemp = io.feed("bbq.prevTemp");
AdafruitIO_Feed *decDeg = io.feed("bbq.decDeg");
AdafruitIO_Feed *incDeg = io.feed("bbq.incDeg");
AdafruitIO_Feed *minDecr = io.feed("bbq.minDecr");
AdafruitIO_Feed *minIncr = io.feed("bbq.minIncr");


// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Example creating a thermocouple instance with hardware SPI
// on a given CS pin.
//#define MAXCS   0
//Adafruit_MAX31855 thermocouple(MAXCS);

// Example creating a thermocouple instance with hardware SPI
// on SPI1 using specified CS pin.
//#define MAXCS   4
//Adafruit_MAX31855 thermocouple(MAXCS, SPI1);

void setup() {
  Serial.begin(9600);
  io.connect();
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  
  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");
}

int previousTemp = 0;
int currentTemp = 0;
int minutesDecreasing = 0;
int minutesIncreasing = 0;
int decreasedTempTtl = 0;
int increasedTempTtl = 0;

void loop() {
      io.run();

  currentTemp = thermocouple.readFahrenheit();
  if (currentTemp < 1000 && currentTemp > -1000) {
    
  
   // basic readout test, just print the current temp
    calcTemp(currentTemp); 
    curTemp->save(currentTemp);
    prevTemp->save(previousTemp);
    decDeg->save(decreasedTempTtl);
    incDeg->save(increasedTempTtl);
    minDecr->save(minutesDecreasing);
    minIncr->save(minutesIncreasing);


    debugPrint();

    
    previousTemp = currentTemp;
  
   delay(1000 * 60);
  } 
}

void debugPrint(){
   char buffer[128];
  sprintf(buffer, "curtemp: %d prevtemp: %d decmin: %d  decttldeg: %d  incmin: %d  incttldeg: %d", currentTemp, previousTemp, minutesDecreasing, decreasedTempTtl, minutesIncreasing, increasedTempTtl);
  Serial.println(buffer);
  
}
void calcTemp(int currentTemp){

  int diff = floor(currentTemp - previousTemp);
  if (currentTemp > previousTemp){incrementIncreased(diff);}
  if (currentTemp < previousTemp){incrementDecreased(diff);}
}

void incrementDecreased(int tempDiff){
  resetIncreasing();
  minutesDecreasing += 1;
  decreasedTempTtl += abs(tempDiff);
}

void incrementIncreased(int tempDiff){
  resetDecreasing();
  minutesIncreasing += 1;
  increasedTempTtl += abs(tempDiff);
}

void resetIncreasing(){

increasedTempTtl = 0;
minutesIncreasing = 0;
 
}

void resetDecreasing(){
  decreasedTempTtl = 0;
  minutesDecreasing = 0;
}
