/*
 * Created by Miguel Angel Calvera (Miguel33Angel)
 * 
 * TODO: 
 * - Resolve small TODO in code
 * - Check full working conditions: Multiple cards, maximum amount of cards and time running without errors in days.
 * - Check if there's no Debug message residue from updating
 * This are improvements so low priority
 * - Change Web GUI. It's horribly small and should be more clear -> In progress
 * - Change Webserver handling. Code horrible to revise or refactor
 * - Change SPIFFS type of system to LITTELFS. It's just installing the library and changing all the SPIFFS.something o LITTELFS.something.
*/


// 


// //1 HTTP/1.1 -> 1
// //Tarjeta+universidad HTTP/1.1 -> Tarjeta+universidad
// //Gets array, lenght, n1 and deletes last n1 char. (9)
// byte deleteLastCharOfArr(char* arr, byte n, byte last_char){
//   byte j=0;
//   for(byte i=0; i<n; i++){
//     if(i<(n-last_char)){
//         //arr[j]=arr[i]; As I'm not deleting elements, just making list shorter this is not needed.
//         j++;
//       }
//   }
//   n=j;
//   return n;
// }
  


// //********************************************************************************************************

// void setup() {
//   pinMode(PIN_RELAY, OUTPUT);
//   digitalWrite(PIN_RELAY, LOW);
//   if (DEBUG == 1){
//     Serial.begin(115200);
//   }

//   //Spiffs setup
//   if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
//     debugln(F("SPIFFS Mount Failed"));
//     restart();
//   }

//   //Wi-Fi
//   debugln();
//   debugln(F("Configuring access point..."));
  
//   // You can remove the password parameter if you want the AP to be open.
//   WiFi.softAP(ssid, password);
//   IPAddress myIP = WiFi.softAPIP();
//   debug(F("AP IP address: "));
//   debugln(myIP);
//   server.begin();

//   debugln(F("Server started"));

//   //Now, do the NFC
  
//   SPI.begin(); // init SPI bus
//   rfid.PCD_Init(); // init MFRC522
//   debugln(F("Tap RFID/NFC Tag on reader"));

// }

// void loop() {
//   now = millis();
//   if(now - last_time_checked>14400000){ //3600000
//     //If more than 60 minutes has passed, then restart
//     rfid.PCD_PerformSelfTest();
//     last_time_checked = millis();
//   }

// //  if(now - last_time_checked>115200000){ //3600000
// //    //If more than 60 minutes has passed, then restart
// //    restart();
// //    last_time_checked = millis();
// //  }

  
//   //Priority is to read the NFC cards, in case a auth card get's readed.
//   //If unauth, then it should be saved in case we want to save it
//   if (rfid.PICC_IsNewCardPresent()) {   // if new tag is available
//     if (rfid.PICC_ReadCardSerial()) {   // and if NUID has been readed
//       if(isValidUID(rfid.uid.uidByte)){ //It's an AUTH UID:
//         debug(F("Authorized Tag with UID:"));
//         //TODO: Do Relay control with other core.
//         digitalWrite(PIN_RELAY, HIGH);   // turn the LED on (HIGH is the voltage level)
//         delay(2000);                       // wait for a second
//         digitalWrite(PIN_RELAY, LOW);    // turn the LED off by making the voltage LOW
//       }else{                             //It's an UNAUTH UID:
//         debug(F("Unauthorized Tag with UID:"));
//         for (byte i=0;i<UID_SIZE;i++){ //Save the uid in lastUnauthUID
//           lastUnauthUID[i] = rfid.uid.uidByte[i];
//           }
//         newUnauthCard = true;
//       }

//       //Print the uid readed
//       printHex(rfid.uid.uidByte);
//       debugln();
      
//       rfid.PICC_HaltA(); // halt PICC
//       rfid.PCD_StopCrypto1(); // stop encryption on PCD
//     }
//   }else{
//     //Now listen for incoming clients
//     WiFiClient client = server.available();  
//     if (client) {                             // if you get a client,
//       debugln(F("New Client."));
//       // currentLineBuffer-> make a String to hold incoming data from the client
//       nCurrent = 0; //This makes currentLine to be empty
//       while (client.connected()) {            // loop while the client's connected
//         if (client.available()) {             // if there's bytes to read from the client,
//           char c = client.read();             // read a byte, then
//           // debug(c);                    // print it out the serial monitor
//           if (c == '\n') {                    // if the byte is a newline character

//             // if the current line is blank, you got two newline characters in a row.
//             // that's the end of the client HTTP request, so send a response:
//             if (nCurrent == 0) {
//               // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
//               // and a content-type so the client knows what's coming, then a blank line:
//               client.println(F("HTTP/1.1 200 OK"));
//               client.println(F("Content-type:text/html"));
//               client.println();
//               // the content of the HTTP response follows the header:
            
//               // Pre makes so spaces are still saved. Style makes so the font is bigger.
//               client.print(F("<pre style=\"font-size:20px;\">"));
            
//               //Now the content in itself. We need to open the files to get the display of the data.
//               File memoryUID = SPIFFS.open(UID_PATH, FILE_READ);
//               File memoryNAMES = SPIFFS.open(NAMES_PATH, FILE_READ);

//               if(!memoryUID or !memoryNAMES){
//                 debugln(F("- failed to open one file"));
//                 restart();
//               }
            
//               byte i = 0 ;
//               while(memoryUID.available() and memoryNAMES.available()){ 
//                 //So, while the files are still unread
//                 client.print(i+1);
//                 client.print(F("  -  "));
              
//                 //Name to be readen
//                 nCurrent = 0; //Makes current empty

//                 char l= 'a';  //letter of the name

//                 while(memoryNAMES.available() and l !='\n'){ //If the character is not the delimitator
//                   l = memoryNAMES.read();                    //Read a letter (char)
//                   if(l=='+'){
//                     l=' ';  
//                   }
//                   nCurrent = addChar(bufferCurrentLine,nCurrent,l);//Add it to the current Name
//                 }
//                 l=' '; //Reset condition for l
//                 for(byte j=0;j<nCurrent-1;j++){//-1 because nCurrent has the \n
//                   client.print(bufferCurrentLine[j]);
//                 }
//                 client.print(F(" - "));
              
//                 byte currentByte;         //Byte of UID to be readen
//                 byte nBytesRead=0;          //Counter
//                 while (memoryUID.available() and nBytesRead < UID_SIZE) {
//                   currentByte = memoryUID.read();
//                   client.print(currentByte < 0x10 ? " 0" : " "); //Puts 0 in numbers like 2,9,B etc
//                   client.print(currentByte, HEX);
//                   nBytesRead++;
//                 }
//                 client.print(F("<br/>"));
//                 i++;
//               }
//               //When done with files, close them:
//               memoryUID.close();
//               memoryNAMES.close();
            
//               client.print(F("</pre>"));
            
//               //Input for adding a new UID
//               //Input for deleting a existing UID
//               client.print(F("<form action=\"http://192.168.4.1/Send_Data/\">"
//               "<label style=\"font-size:25px;\"  for=\"fname\">Add with Name: </label>" 
//               "<input style=\"font-size:20px;\"  type=\"text\" id=\"fname\" name=\"fname\">"
//               "<input style=\"font-size:20px;\"  type=\"submit\" value=\"Add\"> </form>"));
//               client.print(F("<form action=\"http://192.168.4.1/Delete_Data/\">"
//               "<label style=\"font-size:25px;\"  for=\"num\">Delete number: </label>" 
//               "<input style=\"font-size:20px;\"  type=\"text\" id=\"num\" name=\"num\">"
//               "<input style=\"font-size:20px;\"  type=\"submit\" value=\"Delete\"> </form>"));

//               // The HTTP response ends with another blank line:
//               client.println();
//               // break out of the while loop:
//               break;
//             } else {    // if you got a newline, then clear currentLine and savedData (in case we are saving data):
//               if (saveNextData){ //If this is true, it's because we got a response from the phone saying "GET /Send_Data/_somedata_" and we want to use _somedata_
//                 if (Data_Action==add){
//                   nSaved = deleteLastCharOfArr(bufferSavedData,nSaved,DEL_LAST_CHAR);
//                   printHex(lastUnauthUID);
//                   AddUser(lastUnauthUID, bufferSavedData, nSaved);
//                   saveNextData = false;
                
//                 }else if (Data_Action==del){
//                   nSaved = deleteLastCharOfArr(bufferSavedData,nSaved,DEL_LAST_CHAR);
//                   deleteUser(charArrToInt(bufferSavedData,nSaved));
//                 }
              
//                 saveNextData = false; //Finish saving the message.
//                 nSaved = 0; //Delete it after used
//               }
//               nCurrent = 0;
//             }
//           } else if (c != '\r') {  // if you got anything else but a carriage return character,
//             nCurrent = addChar(bufferCurrentLine, nCurrent, c);      // add it to the end of the currentLine
//             if(saveNextData){      // and to savedData, in case we are interested
//               nSaved = addChar(bufferSavedData, nSaved, c);
//             }
//           }
            
//           if (ArrIsString(bufferCurrentLine,nCurrent,"GET /Send_Data/?fname=")){ 
//             //Start to save the data incomming until you get the whole message
//             saveNextData = true;
//             Data_Action = add;
          
//           }else if (ArrIsString(bufferCurrentLine,nCurrent,"GET /Delete_Data/?num=")){
//             //Start to save the data incomming until you get the whole message
//             saveNextData = true; 
//             Data_Action = del;
//           }
        
//         }
//       }//End While client is connected
//       // close the connection:
//       client.stop();
//       debugln(F("Client Disconnected."));
//     }
//   }
// }

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/stepper-motor-esp32-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Debug definitions
#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// General includes
#include <Arduino.h>

// Includes for the Wifi access point
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//Defines for RELAY
#define PIN_RELAY 4

// #define DEL_LAST_CHAR 9
// #define REFERER_FIRST_DATA_CHAR 28


// Includes for the NFC card reader
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5  // ESP32 pin GIOP5 
//#define RST_PIN 27 // ESP32 pin GIOP27 
#define RST_PIN 21 

#define UID_SIZE 4 //UID are going to be 4 long

//Includes for using the Flash memory
#include "FS.h"
//#include <LittleFS.h>
#include "SPIFFS.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me−no−dev/arduino−esp32fs−plugin */
#define FORMAT_SPIFFS_IF_FAILED true

#define UID_PATH "/UID.txt"
#define NAMES_PATH "/Names.txt"

#define UID_PATH_TEMP "/UID_temp.txt"
#define NAMES_PATH_TEMP "/Names_temp.txt"


//Variables for the SPIFFS file system

// Variables for the Wifi access point
// Replace with your network credentials
const char* ssid = "yourSSIDname";
const char* password = "yourpassword";

// const char *ssid = "San Pedro";
// const char *password = "Sergiopass";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameters in HTTP POST request
const char* PARAM_INPUT_1 = "num";
const char* PARAM_INPUT_2 = "fname";

enum action {add, del, not_defined}; //add=0, del=1;
enum action Data_Action;

// Variables for the NFC card reader
MFRC522 rfid(SS_PIN, RST_PIN);

byte lastUnauthUID[UID_SIZE] = {0x00, 0x00, 0x00, 0x00};
bool newUnauthCard = false;

//V2 auto-check;
// byte VersionRFID=0;
// unsigned long  last_time_checked=0;
// unsigned long now=0;



// Variables to save values from HTML form
#define BUFFER_SIZE 50
char number_to_delete[BUFFER_SIZE];
byte size_delete_arr=0;
char name_to_add[BUFFER_SIZE];
byte size_add_arr=0;
// Variable to detect whether a new request occurred
bool newRequest = false;

char info_html[150];

// HTML to build the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Door Card List</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <h1>Door Card List</h1>
    <form action="/deleteButtonPressed" method="POST">
      <label for="steps">Delete Number:</label>
      <input type="text" name="num">
      <input type="submit" value="DELETE!">
    </form>
    <br><br><br>
    <form action="/addButtonPressed" method="POST">
      <label for="steps">Add last card with Name:</label>
      <input type="text" name="fname">
      <input type="submit" value="ADD!">
    </form>
</body>
</html>
)rawliteral";


//
//
//*****************************FUNCTIONS DEFINITIONS**********************************************
void restart(){
  debugln("ESP Restarting...");
  delay(100);
  ESP.restart();
}

//Convert char array to a number.
int charArrToInt(char* arr, byte n){
  int r=0;
  for(byte i=0; i < n; i++){
    r = 10*r + (arr[i]-'0');
  }
  return r;
}


byte addChar(char* arr, byte n, char c){
  if(n<BUFFER_SIZE){
    arr[n]=c;
    n=n+1;

  }
  return n;
}

bool ArrIsString(char* arr, byte n, String s){
  bool r = (n == s.length());
  byte i=0;
  while(r and i<n){
    r = s.charAt(i) == arr[i];
    i++;
  }
  return r;
}

void printArr(char *buffer, byte n){
  for (byte i = 0; i < n; i++) {
    if (DEBUG == 1){
      Serial.print(buffer[i]);
    }
  }
  Serial.println();
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
//Doesn't make sense for example to check the last value if third is wrong
bool isValidUID(byte *UID){
  bool r = false;
  File memoryUID = SPIFFS.open(UID_PATH, FILE_READ); //Spiffs is the type of file system
  
  if(!memoryUID){
    debugln(F("- failed to open file UID in checking if UID is auth or not"));
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

bool AddUser(byte *lastUnauthUID, char* arr, byte n){  //TODO: Fucntion depends on global value, doesn't make sense
  
  if(not newUnauthCard){
    debugln(F("No hay nuevo usuario que añadir"));
    return false;
  }
  File memoryUID = SPIFFS.open(UID_PATH, FILE_APPEND);
  File memoryNAMES = SPIFFS.open(NAMES_PATH, FILE_APPEND);

  if(!memoryUID or !memoryNAMES){
    debugln(F("- failed to open files while adding user"));
    return false;
  }
  printArr(arr,n);
  for(byte i=0;i<UID_SIZE;i++){
    memoryUID.write(lastUnauthUID[i]);
  }
  for(byte i=0;i<n;i++){
    memoryNAMES.write(arr[i]);
  }
  memoryNAMES.print('\n');

  memoryUID.close();
  memoryNAMES.close();

  newUnauthCard = false;
  
  return true;
}

bool deleteUser(int del_n){
  debug(del_n);
  del_n = del_n-1;
  if(del_n<0){
    debugln(F("Invalid number to delete"));
    return false;
  }
  File memoryUID = SPIFFS.open(UID_PATH, FILE_READ);
  File tempUIDfile = SPIFFS.open(UID_PATH_TEMP, FILE_WRITE);

  if(!memoryUID or !tempUIDfile){
    debugln(F("- failed to open UID files while deleting user"));
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
    debugln(F("- failed to open NAMES files while deleting user"));
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

//Creates list of names as well as the button to add last RFID card
bool createResponse(AsyncResponseStream *response){
  response->addHeader("Server","Añadir a alguien a la lista");
  response->printf("<!DOCTYPE html><html><head><title>Añadir a alguien a la lista</title></head><body>");
  response->print("<h1>Lista de personas con acceso: ");
  response->print("</h1>");
  response->print("</body></html>");
  response->print("<pre style=\"font-size:20px;\">");

  //Now the content in itself. We need to open the files to get the display of the data.
  File memoryUID = SPIFFS.open(UID_PATH);
  File memoryNAMES = SPIFFS.open(NAMES_PATH);

  if(!memoryUID or !memoryNAMES){
    debugln(F("- failed to open one file in createResponse"));
    return false;
  }

  byte i = 0 ;
  while(memoryUID.available() and memoryNAMES.available()){ 
    //So, while the files are still unread
    response->printf("%i  -  ",i+1);
  
    //Name to be readen
    char l= 'a';  //letter of the name

    while(memoryNAMES.available() and l !='\n'){ //If the character is not the delimitator
      l = memoryNAMES.read();                    //Read a letter (char)
      if(l=='+'){ //spaces are encoded as +
        l=' ';  
      }
      if(l=='\n'){
        l=' ';
      }
      response->print(l);
      debug(l);
    }
    l=' '; //Reset condition for l
    response->print("- ");
  
    byte currentByte;         //Byte of UID to be readen
    byte nBytesRead=0;          //Counter
    while (memoryUID.available() and nBytesRead < UID_SIZE) {
      currentByte = memoryUID.read();
      response->print(currentByte < 0x10 ? " 0" : " "); //Puts 0 in numbers like 2,9,B etc
      response->print(currentByte, HEX);
      nBytesRead++;
    }
    response->print("<br/>");
    i++;
  }
  //When done with files, close them:
  memoryUID.close();
  memoryNAMES.close();
  response -> print("</pre>");

  return true;
}

//TODO complete add response
bool finishAddResponse(AsyncResponseStream *response){

  return true;
}

//TODO complete delete response
bool finishDeleteResponse(AsyncResponseStream *response){

  return true;
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_AP); //Set mode to AP -> access point. Other options: WIFI_STA or 
  WiFi.softAP(ssid, password); //Config AP, in case of esp32 being connected to wifi -> 
  Serial.print("Creating to WiFi ");
  Serial.println(WiFi.softAPIP());
}

void setup() {
  Serial.begin(115200);

  //Spiffs setup
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    debugln(F("SPIFFS Mount Failed"));
    restart();
  }

  initWiFi();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  server.on("/add", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    createResponse(response);
    finishAddResponse(response);
    request->send(response);
  });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    createResponse(response);
    finishDeleteResponse(response);
    request->send(response);
  });
  
  // Handle request (form) of delete button in home page
  server.on("/deleteButtonPressed", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        // HTTP POST input1 value (number)
        if (p->name() == PARAM_INPUT_1) {
          // number_to_delete = p->value().c_str();
          // C++ garbage semantics
          // p->value() is a fancy way of doing p.value, when p is a pointer to a struct, instead of a struct
          // p->value().c_str() is a fancy way of getting the c type string from the string of value of the structure p is pointing.
          p->value().toCharArray(number_to_delete, BUFFER_SIZE);
          size_delete_arr = p->value().length();
          printArr(number_to_delete,size_delete_arr);
          debugln("First ");
          Data_Action = del;
        }
      }
    }
    if(Data_Action == del){
      request->redirect("/delete");
    }else{
      request->send(200, "text/html", index_html);
    }
    newRequest = true;
    Data_Action = not_defined; //Reset value
  });

  //Now for when user presses add button in home page
  server.on("/addButtonPressed", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        // Only adding matthers
        // HTTP POST input2 value (name)
        if (p->name() == PARAM_INPUT_2) {
          // name_to_add = p->value().c_str();
          p->value().toCharArray(name_to_add, BUFFER_SIZE);
          size_add_arr = p->value().length();
          printArr(name_to_add,size_add_arr);
          debugln("Second ");
          Data_Action = add;
        }
      }
    }

    if(Data_Action == add){
      request->redirect("/add");
    }else{
      request->send(200, "text/html", index_html);
    }

    newRequest = true;
    Data_Action = not_defined; //Reset value
  });

  server.begin();
}

void loop() {
  // Check if there was a new request and move the stepper accordingly
  if (newRequest){
    // if (direction == "CW"){
    //   // Spin the stepper clockwise direction
    //   //myStepper.step(steps.toInt());
    //   Serial.println("CW Direccion");
    // }
    // else{
    //   // Spin the stepper counterclockwise direction
    //   //myStepper.step(-steps.toInt());
    //   Serial.println("La otra Direccion");
    // }
    debugln("New request");
    debugln("");
    newRequest = false;
  }



}
