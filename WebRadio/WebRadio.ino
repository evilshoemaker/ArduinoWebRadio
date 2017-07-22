#include <Ethernet.h>
#include <SPI.h>

struct RadioStation {
  char server [50];
  char url[70];
  int port;
};

RadioStation radioStationList[] = {{"test", "test", 60}};

void setup() {
  
}

void loop() {
  
}

