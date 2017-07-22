#include <Ethernet.h>
#include <SPI.h>
#include <PCD8544_mod.h>
#include <VS1053.h>
#include <TimerOne.h>
//#include <Adafruit_VS1053.h>

#define LED1    A3

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192, 168, 0, 49);
EthernetClient client;

PCD8544 lcd(3, 4, 5, 2, A0); // clk_pin, data_in_pin, data_select_pin, reset_pin, enable_pin
VS1053 player(9, 6, 7, 8); // cs_pin, dcs_pin, dreq_pin, reset_pin

#define BUF_SIZE 1

//#define BREAKOUT_RESET  8      // VS1053 reset pin (output)
//#define BREAKOUT_CS     9     // VS1053 chip select pin (output)
//#define BREAKOUT_DCS    6      // VS1053 Data/command select pin (output)
//#define DREQ 7       // VS1053 Data request, ideally an Interrupt pin

//Adafruit_VS1053 musicPlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ);
//Adafruit_VS1053 musicPlayer(11, 12, 13, 8, 9, 6, 7);//int8_t mosi, int8_t miso, int8_t clk, int8_t rst, int8_t cs, int8_t dcs, int8_t dreq

struct RadioStation {
  char const * server;
  char const * url;
  unsigned int port;
};

const RadioStation radioStationList[] = {
  {"ic2.101.ru", "/a112", 80}
};

int currentRadioStation = 0;
//uint8_t * streamData;
char streamData[128];
int totalBytes = 0;
//uint16_t totalBytes = 0;
int totalReadData = 0;

int initialLoop =0;
int reconnectCounter=0;
int noReceptionCounter =0;

boolean ViewStationInfo = false;
int indexCounter = 0;
static uint32_t timer;
boolean radioStationIsChanged = false;
boolean receivedData = true;

void setup() {
  Serial.begin(57600);
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


//  lcd.begin(84, 48); //84*48 pixels lcd (Nokia 5110).
//  lcd.clear();
//  lcd.setCursor(0, 2);
//  lcd.print(F("-= WEBRADIO PLAYER =-"));
  /*lcd.setCursor(0, 1);
  lcd.print(F("fw: 1.0"));
  lcd.setCursor(0, 2);
  lcd.print(F("(C) 2014 BY"));
  lcd.setCursor(0, 3);
  lcd.print(F("VASSILIS SERASIDIS"));
  lcd.setCursor(0, 5);
  lcd.print(F("WWW.SERASIDIS.GR"));*/

  player.begin();
  player.modeSwitch(); //Change mode from MIDI to MP3 decoding (Vassilis Serasidis).
    // set maximum output volume
  player.setVolume(230);

  //musicPlayer.begin();
  //musicPlayer.setVolume(20,20);

  restartConnection();
  

  /*if (client.connect(radioStationList[0].server, radioStationList[0].port)) {
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
  }*/


  /*ViewStationInfo = true;
        
        Timer1.initialize(5000000); // set a timer of length 5000000 microseconds (or 5 sec => the led will blink 5 times, 5 cycles of on-and-off, per second)
        Timer1.attachInterrupt( timerIsr ); // attach the service routine here*/
  
}

void loop() {
  /*if (client.available() ) {
      totalBytes=client.readBytes(streamData, 32);
      my_callback(streamData, totalBytes);
  }

  
  if(radioStationIsChanged == true) {//If 'next' or 'previous' switch has been pressed, play the selected webradio station.
    playWebRadioStation(radioStationList[0], "01");
  }

  radioStationIsChanged = false; */

  /*if((millis() > timer + 5000)) // Timeout timer. If the song stops playing for 5 seconds re-connect to the server.
  {   
   radioStationIsChanged = true; 
   ViewStationInfo = false;
   Serial.print(F("\nre-connecting to the server...\n"));
   timer = millis(); 
  }*/
  
  /*if (client.available()) {
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
  }*/
  
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
  test();
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

  char clientline[BUF_SIZE];
  int index = 0;
  
//  while (client.connected()) {
//      if (client.available()) {
//        char c = client.read();
//
//        if (index < BUF_SIZE - 1) {
//            clientline[index] = c;
//            index++;
//
//            continue;
//        }
//        clientline[index] = c;
//       
//        
//        // got a \n or \r new line, which means the string is done
//        //clientline[index] = 0;
//        
//        // Print it out for debugging
//        Serial.println(clientline);
//        player.playChunk((uint8_t*)clientline, BUF_SIZE); 
//
//        index = 0;
//      }
//   }

  long startTime = millis();
  
  totalBytes = 0;
  while  (((millis() - startTime ) < 100)) {
    if (client.available() ) {
      totalBytes=client.read((uint8_t *)streamData, 32);
      //totalBytes = client.readBytes(streamData, 32);
      //Serial.println((int)totalBytes, DEC);
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

  player.playChunk((uint8_t *)streamData, totalBytes);   
}

void restartConnection(){
    initialLoop=1;
    reconnectCounter=0;
    totalReadData=0;
    client.flush();
    client.stop(); 
    Serial.print("New Station: "); //Serial.println(ndefServer);
}


static void my_callback (uint8_t * streamData, uint16_t totalBytes)
{
  Serial.print("totalbyte = ");
  Serial.println(totalBytes);
  unsigned int i;
        
        if((indexCounter < 500) /*&& (ViewStationInfo == false)*/)
        {
          Serial.print((char *) streamData);
          
          //for(i=0;i<len;i++)
          {
            /*Serial.write(Ethernet::buffer[off+i]); //Show the web radio channel information (name, genre, bit rate etc).
            if((indexCounter + i > 179) && (indexCounter + i < 301)) 
               lcd.write(Ethernet::buffer[off+i]);
               
            if(Ethernet::buffer[off+i] == 0x0d && 
               Ethernet::buffer[off+i+1] == 0x0a && 
               Ethernet::buffer[off+i+2] == 0x0d && 
               Ethernet::buffer[off+i+3] == 0x0a)
               {
                   ViewStationInfo = true;
                   break; //We found the index end (0x0d,0x0a,0x0d,0x0a). Do not search anymore.
               }*/
          }
          indexCounter += totalBytes;
        }
        else
        {
           //uint8_t* data = c//(uint8_t *) Ethernet::buffer + off; //Get the data stream from ENC28J60 and...
           player.playChunk(streamData, totalBytes);                        //...send them to VS1053B
           timer = millis();                                   //Update the timeout timer.
           receivedData = true;
        }          
}

void playWebRadioStation ( RadioStation station, char* preset )
{
  Serial.print("\n\n<"); //Print the station info to the serial port
  Serial.print(preset);
  Serial.println(F("> ============================================================"));
  
  //player.stopSong();
  ViewStationInfo = false;
  indexCounter = 0;

  if (client.connect(radioStationList[0].server, radioStationList[0].port)) {
    Serial.println("connected");
    // Make a HTTP request:

    client.print("GET "); client.print(radioStationList[0].url); client.println(" HTTP/1.1");
    client.println("Connection: Keep-Alive");
    client.print("HOST: "); client.println(radioStationList[0].server);
    client.println();  
    Serial.println(" - Connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  
  
  /*ether.copyIp(ether.hisip, ip);
  ether.hisport = hisPort;
  ether.printIp("IP:   ", ether.myip);
  ether.printIp("GW:   ", ether.gwip);
  ether.printIp("DNS:  ", ether.dnsip);
  ether.printIp("SRV:  ", ether.hisip); 
  Serial.print("Port: ");
  Serial.println(ether.hisport);
  Serial.println();
  //ether.browseUrl(PSTR("/"), "",PSTR(""), PSTR("Icy-MetaData:1"), my_callback);
  ether.browseUrl(PSTR("/"), "",PSTR(""), PSTR(""), my_callback);
  lcd.clear();
  lcd.setCursor(0, 0); //LCD line 0
  lcd.write('<'); 
  lcd.print(preset); //Show on LCD the webradio number (1-14)
  lcd.write('>');
  lcd.write(' ');*/
}

void timerIsr()
{
    // Toggle LED
    digitalWrite( LED1, digitalRead( LED1 ) ^ 1 );
    if(receivedData == true)
      receivedData = false;
    else
    {
      radioStationIsChanged = true; 

      client.flush();
      client.stop(); 
      
      //if (Ethernet.begin(mac) == 0) //Initiallize ENC28J60. Chip Select pin (CS) is connected on arduino pin D10.
      //  Serial.println(F("Failed to access Ethernet controller"));
      /*if (!ether.dhcpSetup())
        Serial.println(F("DHCP failed"));
      ether.persistTcpConnection(true); //Keep TCP/IP connection alive.*/
    }      
}

