#include <Ethernet.h>
#include <SPI.h>
#include <PCD8544_mod.h>
//#include <VS1053.h>
#include <Adafruit_VS1053.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192, 168, 0, 49);
EthernetClient client;

PCD8544 lcd(3, 4, 5, 2, A0); // clk_pin, data_in_pin, data_select_pin, reset_pin, enable_pin
//VS1053 player(9, 6, 7, 8); // cs_pin, dcs_pin, dreq_pin, reset_pin

//#define BREAKOUT_RESET  8      // VS1053 reset pin (output)
//#define BREAKOUT_CS     9     // VS1053 chip select pin (output)
//#define BREAKOUT_DCS    6      // VS1053 Data/command select pin (output)
//#define DREQ 7       // VS1053 Data request, ideally an Interrupt pin

//Adafruit_VS1053 musicPlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ);
Adafruit_VS1053 musicPlayer(11, 12, 13, 8, 9, 6, 7);//int8_t mosi, int8_t miso, int8_t clk, int8_t rst, int8_t cs, int8_t dcs, int8_t dreq

struct RadioStation {
  char const * server;
  char const * url;
  unsigned int port;
};

const RadioStation radioStationList[] = {
  {"205.164.62.15", "/", 10032}
};

int currentRadioStation = 0;
uint8_t * streamData;
uint16_t totalBytes = 0;
int totalReadData = 0;

int initialLoop =0;
int reconnectCounter=0;
int noReceptionCounter =0;



void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Web-radio initialization");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  delay(1000);
  Serial.println("connecting...");


  lcd.begin(84, 48); //84*48 pixels lcd (Nokia 5110).
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print(F("-= WEBRADIO PLAYER =-"));
  /*lcd.setCursor(0, 1);
  lcd.print(F("fw: 1.0"));
  lcd.setCursor(0, 2);
  lcd.print(F("(C) 2014 BY"));
  lcd.setCursor(0, 3);
  lcd.print(F("VASSILIS SERASIDIS"));
  lcd.setCursor(0, 5);
  lcd.print(F("WWW.SERASIDIS.GR"));*/

  //player.begin();
  //player.modeSwitch(); //Change mode from MIDI to MP3 decoding (Vassilis Serasidis).
    // set maximum output volume
  //player.setVolume(0x00);

  musicPlayer.begin();
  musicPlayer.setVolume(20,20);

  //restartConnection();
  

  if (client.connect(radioStationList[0].server, radioStationList[0].port)) {
    Serial.println("connected");
    // Make a HTTP request:

    client.print("GET "); client.print(radioStationList[0].url); client.println(" HTTP/1.1");
    client.println("Connection: Keep-Alive");
    client.print("HOST: "); client.println(radioStationList[0].server);
    client.println(); 
    
    //client.println("GET / HTTP/1.1\r\n");
    //client.println("\r\n");
    //client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  
}

void loop() {
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    //totalBytes=client.read(streamData, 32);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
  
  /*unsigned long startTime = millis();
  
  totalBytes=0;
  while  (((millis() - startTime ) < 100)) {
    if (client.available() ) {
      totalBytes=client.read(streamData, 32);
      break;
    }
  }
  if (totalBytes==0) {
    Serial.print(".");  
    return;
  } 
  else {
    //noReceptionCounter=0;
    if (totalBytes != 32) Serial.print("+"); 
    totalReadData += totalBytes;
    if (totalReadData > 10000) {
      Serial.print("*"); 
      totalReadData=0;
    }
  }   
  player.playChunk(streamData, totalBytes); */
  //MP3player.transferStream(totalBytes,streamData);
  //test();
}

void test() {
  if (initialLoop==1) { 
    noReceptionCounter=0;
    if (client.connect(radioStationList[0].server, radioStationList[0].port)) {
        client.print("GET "); client.print(radioStationList[0].url); client.println(" HTTP/1.1");
        client.println("Connection: Keep-Alive");
        client.print("HOST: "); client.println(radioStationList[0].server);
        client.println();  
        Serial.println(" - Connected");
        initialLoop=2;    
    } else {
        Serial.println("Error: not connected");
        reconnectCounter++;
        if (reconnectCounter>=3) {
          Serial.println("Selecting Next Station");
          //nextStation();
          restartConnection();
          return;
        }
        delay(5000);
        return;
    }

  }

  long startTime = millis();
  
  totalBytes = 0;
  while  (((millis() - startTime ) < 100)) {
    if (client.available() ) {
      totalBytes = client.read(streamData, 32);
      //Serial.println(totalBytes);
      //Serial.write(streamData, totalBytes);
      break;
    }
  }
  if (totalBytes==0) {
    Serial.print(".");  
    noReceptionCounter++;
    if (noReceptionCounter == 100) {
      restartConnection();
    }
    return;
  } else {
    noReceptionCounter=0;
    if (totalBytes!=32) Serial.print("+"); 
    totalReadData+=totalBytes;
    if (totalReadData>10000) {
      Serial.print("*"); 
      totalReadData=0;
    }
  }

  //player.playChunk(streamData, totalBytes); 
  musicPlayer.playData(streamData, totalBytes);
}

void restartConnection(){
    initialLoop=1;
    reconnectCounter=0;
    totalReadData=0;
    client.flush();
    client.stop(); 
    Serial.print("New Station: ");
}

