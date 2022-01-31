/*
 * Created by Miguel Angel Calvera (Miguel33Angel)
 * 
*/

/*
 * TODO: (This are improvements so low priority)
 * - Change all String to *char: create functions like endsWith and similar
 * - Change SPIFFS type of system to LITTELFS. It's just installing the library and changin all the SPIFFS.something o LITTELFS.something.
*/

//Debug definitions
#define DEBUG 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

//Defines for RELAY
#define PIN_RELAY 4


// Includes for the Wifi access point
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define ADD_FIRST_CHAR 8
#define ADD_LAST_CHAR 9
#define DEL_FIRST_CHAR 6
#define DEL_LAST_CHAR 9


// Includes for the NFC card reader
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5  // ESP32 pin GIOP5 
#define RST_PIN 27 // ESP32 pin GIOP27 

#define UID_SIZE 4 //UID are going to be 4 long

//Includes for using the Flash memory
#include "FS.h"
#include "SPIFFS.h"

#define UID_PATH "/UID.txt"
#define NAMES_PATH "/Names.txt"

#define UID_PATH_TEMP "/UID_temp.txt"
#define NAMES_PATH_TEMP "/Names_temp.txt"

#define FORMAT_SPIFFS_IF_FAILED true

//Variables for the SPIFFS file system

// Variables for the Wifi access point
// Set these to your desired credentials.
WiFiServer server(80);
const char *ssid = "yourAP";
const char *password = "yourPassword";

bool saveNextData = false; //To check if we need to save the data the user is sending through the webpage
String savedData  = "";

enum action {add, del}; //add=0, del=1;
enum action Data_Action;

// Variables for the NFC card reader
MFRC522 rfid(SS_PIN, RST_PIN);

byte lastUnauthUID[UID_SIZE] = {0x00, 0x00, 0x00, 0x00};
bool newUnauthCard = false;


void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH);
  if (DEBUG == 1){
    Serial.begin(115200);
  }
  //Spiffs setup
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    debugln("SPIFFS Mount Failed");
    restart();
  }

  

  //Wi-Fi
  debugln();
  debugln("Configuring access point...");
  
  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  debug("AP IP address: ");
  debugln(myIP);
  server.begin();

  debugln("Server started");

  //Now, do the NFC
  
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  debugln("Tap RFID/NFC Tag on reader");

  

}

void loop() {
  
  
  //Priority is to read the NFC cards, in case a auth card get's readed.
  //If unauth, then it should be saved in case we want to save it
  if (rfid.PICC_IsNewCardPresent()) {   // if new tag is available
    if (rfid.PICC_ReadCardSerial()) {   // and if NUID has been readed
      if(isValidUID(rfid.uid.uidByte)){ //It's an AUTH UID:
        debug("Authorized Tag with UID:");
        digitalWrite(PIN_RELAY, LOW);   // turn the LED on (HIGH is the voltage level)
        delay(2000);                       // wait for a second
        digitalWrite(PIN_RELAY, HIGH);    // turn the LED off by making the voltage LOW
      }else{                             //It's an UNAUTH UID:
        debug("Unauthorized Tag with UID:");
        for (byte i=0;i<UID_SIZE;i++){ //Save the uid in lastUnauthUID
          lastUnauthUID[i] = rfid.uid.uidByte[i];
          }
        newUnauthCard = true;
      }

      //Print the uid readed
      printHex(rfid.uid.uidByte);
      debugln();
      
      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }else{



  //Now listen for incoming clients
  WiFiClient client = server.available();  
  if (client) {                             // if you get a client,
    debugln("New Client.");          
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        debug(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            // the content of the HTTP response follows the header:
            
            // Pre makes so spaces are still saved. Style makes so the font is bigger.
            client.print("<pre style=\"font-size:20px;\">");
            
            //Now the content in itself. We need to open the files to get the display of the data.
            File memoryUID = SPIFFS.open(UID_PATH, FILE_READ);
            File memoryNAMES = SPIFFS.open(NAMES_PATH, FILE_READ);

            if(!memoryUID or !memoryNAMES){
              debugln("- failed to open one file");
              restart();
            }
            
            byte i = 0 ;
            while(memoryUID.available() and memoryNAMES.available()){ 
              //So, while the files are still unread
              client.print(i+1);
              client.print("  -  ");
              
              String currentName = "";  //Name to be readen
              char l= 'a';                   //letter of the name

              while(memoryNAMES.available() and l !='\n'){ //If the character is not the delimitator
                l = memoryNAMES.read();                    //Read a letter (char)
                if(l=='+'){
                  l=' ';  
                }
                currentName = currentName+l;          //Add it to the current Name
              }
              l=' '; //Reset condition for l
              
              client.print(currentName.substring(0,currentName.length()-1)); //Put the name to be displayed
              client.print(" - ");
              
              byte currentByte;         //Byte of UID to be readen
              byte nBytesRead=0;          //Counter
              while (memoryUID.available() and nBytesRead < UID_SIZE) {
                currentByte = memoryUID.read();
                client.print(currentByte < 0x10 ? " 0" : " "); //Puts 0 in numbers like 2,9,B etc
                client.print(currentByte, HEX);
                nBytesRead++;
              }
              client.print("<br/>");
              i++;
            }
            //When done with files, close them:
            memoryUID.close();
            memoryNAMES.close();
            
            client.print("</pre>");
            
            //Input for adding a new UID
            //Input for deleting a existing UID
            client.print("<form action=\"http://192.168.4.1/Send_Data/\">"
            "<label style=\"font-size:25px;\"  for=\"fname\">Add with Name: </label>" 
            "<input style=\"font-size:20px;\"  type=\"text\" id=\"fname\" name=\"fname\">"
            "<input style=\"font-size:20px;\"  type=\"submit\" value=\"Add\"> </form>");
            client.print("<form action=\"http://192.168.4.1/Delete_Data/\">"
            "<label style=\"font-size:25px;\"  for=\"num\">Delete number: </label>" 
            "<input style=\"font-size:20px;\"  type=\"text\" id=\"num\" name=\"num\">"
            "<input style=\"font-size:20px;\"  type=\"submit\" value=\"Delete\"> </form>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine and savedData (in case we are saving data):
            if (saveNextData){ //If this is true, it's because we got a response from the phone saying "GET /Send_Data/_somedata_" and we want to use _somedata_
              //savedData = currentLine.substring(0) ; //Copies currentLine into savedData. Only use if both things are strings
              
              if (Data_Action==add){
                String nameData = savedData.substring(ADD_FIRST_CHAR, savedData.length()-ADD_LAST_CHAR);
                debug("++++"+nameData+"+++++");
                printHex(lastUnauthUID);
                AddUser(lastUnauthUID, nameData);
                
              }else if (Data_Action==del){
                String nameData = savedData.substring(DEL_FIRST_CHAR, savedData.length()-DEL_LAST_CHAR);
                debug("----"+nameData+"-----");
                deleteUser(nameData.toInt());
              }
              debugln();
              
              saveNextData = false; //Finish saving the message.
              savedData = ""; //Delete it after used
              }
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
          if(saveNextData){      // and to savedData, in case we are interested
            savedData += c ;
            }
        }

        // Check the client request:
        if (currentLine.endsWith("GET /Send_Data")) {
          //Start to save the data incomming until you get the whole message
          saveNextData = true; 
          Data_Action = add;
          
        }else if (currentLine.endsWith("GET /Delete_Data")) {
          //Start to save the data incomming until you get the whole message
          saveNextData = true; 
          Data_Action = del;
        }
        
      }
    }//End While client is connected
    // close the connection:
    client.stop();
    debugln("Client Disconnected.");
  }


  }
}

//Function for Hex bytes of the NFC
void printHex(byte *buffer) {
  for (byte i = 0; i < UID_SIZE; i++) {
    if (DEBUG == 1){
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
    }
  }
}

bool isSameUID(byte *UID, byte *AuthUID) {
  bool r = false;
  byte i = 0;
  while(not r and i < UID_SIZE){
      r = AuthUID[i] == UID[i];
      i++;
    }
  return r;
}

//Uses file "UID_PATH" with stored authorizedUID to check against
//memoryUID doesn't have delimitators. Every byte is followed by the next one
//Every i is for every 4 values of the file memoryUID.
//Doesn't make sense for example to skip checking the last value if third is wrong
bool isValidUID(byte *UID){
  bool r = false;
  File memoryUID = SPIFFS.open(UID_PATH, FILE_READ); //Spiffs is the type of file system
  
  if(!memoryUID){
    debugln("- failed to open file UID in checking if UID is auth or not");
    return false;
  }
  
  byte authorizedUID [UID_SIZE];
  byte i=0;
  //available() tells us if the file still has info. With it no need to check number of UIDs.
  while(not r and memoryUID.available()){
      i=0;
      while(i<UID_SIZE and memoryUID.available()){ 
        authorizedUID[i]= memoryUID.read();
        i+=1;
      }
      r = isSameUID(UID, authorizedUID);
  }
  memoryUID.close();
  return r;
}

//Returns true if executes correctly
bool AddUser(byte *lastUnauthUID, String nameData){
  if(not newUnauthCard){
    debugln("No hay nuevo usuario que añadir");
    return false;
  }
  File memoryUID = SPIFFS.open(UID_PATH, FILE_APPEND);
  File memoryNAMES = SPIFFS.open(NAMES_PATH, FILE_APPEND);

  if(!memoryUID or !memoryNAMES){
    debugln("- failed to open files while adding user");
    return false;
  }
  
  for(byte i=0;i<UID_SIZE;i++){
    memoryUID.write(lastUnauthUID[i]);
  }
  memoryNAMES.print(nameData+'\n');

  memoryUID.close();
  memoryNAMES.close();

  newUnauthCard = false;
  
  debug(" Freeding up space. FreeHeap: ");
  debugln(ESP.getFreeHeap());
  return true;
}

bool deleteUser(int del_n){
  del_n--; //For some reason ToInt returns a 0 if an error ocurred, so we have to shift everything by 1, so the numbers displayed will have +1 for the user
  if(del_n==-1){
    debugln("Invalid number to delete");
    return false;
  }
  File memoryUID = SPIFFS.open(UID_PATH, FILE_READ);
  File tempUIDfile = SPIFFS.open(UID_PATH_TEMP, FILE_WRITE);

  if(!memoryUID or !tempUIDfile){
    debugln("- failed to open UID files while deleting user");
    return false;
  }
  byte n=0;
  
  byte i=0;
  byte b=0;
  //available() tells us if the file still has info to read
  while(memoryUID.available()){
    i=0;
    //If n it's different than del_n then read bytes and write them into the other file. If they are the same, just skip those 4 bytes
    while(i<UID_SIZE and memoryUID.available()){ 
        //We always have to read the bytes, even when n == del_d 
        b = memoryUID.read();
        if(n != del_n){ 
          tempUIDfile.write(b);
        }
        i+=1;
    }
    n++;
  }
  memoryUID.close();
  tempUIDfile.close();
  //tempUIDfile has been constructed

  File memoryNAMES = SPIFFS.open(NAMES_PATH, FILE_READ);
  File tempNAMESfile = SPIFFS.open(NAMES_PATH_TEMP, FILE_WRITE);

  if(!memoryNAMES or !tempNAMESfile){
    debugln("- failed to open NAMES files while deleting user");
    return false;
  }
  
  char c;
  n=0;
  //available() tells us if the file still has info to read
  while(memoryNAMES.available()){
    while(memoryNAMES.available() and c !='\n'){ //So when we see a char that is '\0' then the string has ended-> write it down then sum 1 to n, and continue
      c = memoryNAMES.read();
      if(n != del_n){
        tempNAMESfile.write(c);
      }
    }
    c =' '; //Stop c from being '\0' to continue reading
    n++;
  }

  memoryNAMES.close();
  tempNAMESfile.close();

  //tempNAMESfile has been constructed
  //Now we do the critical part. 1 Delete file NAMES_PATH. 2 Change names of NAMES_PATH_TEMP to NAMES_PATH. Same with UID. If it's interrupted after deleting it's gonna lose info.
  //TODO check if operations have been done succesfully, and do exception case. Return false from function.
  SPIFFS.remove(NAMES_PATH);
  SPIFFS.remove(UID_PATH);
  SPIFFS.rename(UID_PATH_TEMP, UID_PATH);
  SPIFFS.rename(NAMES_PATH_TEMP, NAMES_PATH);
  
  return true;
}


void restart(){
  debugln("ESP Restarting...");
  delay(100);
  ESP.restart();
}
