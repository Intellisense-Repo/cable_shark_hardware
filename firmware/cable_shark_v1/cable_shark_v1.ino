/*

 1/4/25   Add the Stop buttion notification command using beep
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string.h>
#include <stdlib.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include "EEPROM.h"
/// Pin define the hardware 
#define relay   9           
#define reset_btn 15
#define door    12        
#define buzzer   2      
//#define rgb_led  38              
#define led_g   17
#define led_b   16
#define led_r   18

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;     
bool oldDeviceConnected = false;  
int button_status = 1;
uint8_t txValue = 0;
String ble_name = "nill";
int loop_hold = 0;
int btn_status = 1;

int door_status = 0;         
bool door_open = false;        
char device_mode = 'A';        

int raw_data = 0;
uint8_t id = 0;     
uint8_t fp_id = 0;  
uint8_t found =  0;
uint32_t chipId = 0;             
uint64_t chipid1;               
char ssid1[15];
char ssid2[15];
uint64_t macid = 0;

String device_pwd;
char psw_buffar[7];

String fp_id_1 = "70";             // 
String fp_id_2 = "75";            // 
String fp_id_3 = "80";            // 
int fp_add = 6;                   // 

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
uint8_t tick_value = 0;

char device_con = 'D';          //
char vehicle_status = 'S';       // 

String mac_string = "";           // 
String new_string = "";           // 
String message = "Ok FINE";  //  
String fine = "FINE";        // 
String message1 = "";
String ide = "CS_$";         
String sucess = "1";
String uuiidd = "";

int device_config = 1;        // 
bool finger_match = false;    // 
bool finger_store = false;
bool finger_delete = false;
char incomingData[10];   
int indx = 0;           
uint8_t std_data[10];
int address = 1;        


  
#define EEPROM_SIZE 60       

//////

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"            //  service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  //  Rx chacterstic UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  //  Tx chacterstic UUID

#define BLE_APPEARANCE_GENERIC_HID 960  // generic HID symbol

///// BLE commands

enum ble_command
{ 
    config_done = 101 ,  deviate_done = 303,    confirm_done =  301,   panicon_done = 102,  panicoff_done =   103,
    nrstart_done = 104,  nrstop_done = 105,     desti_done = 302,      vstart_done = 316,   vstop_done = 317,
    bio_match = 311,     bio_nomatch  = 312,    timeout  = 310,        facematch  = 304,    facenomatch =  305,
    fpenroll_done = 200, fpempty_done = 204,    fpdelete_done = 203,   fp_store  = 201,     d_close = 307,
    d_open = 306,        reset_mode  = 315,     dpsw_done  = 107,      newpsw_done = 106,   device_connect = 100,
    route_wait = 318,    ok_fine = 111,         bio_scan = 310,        reset_done = 308,    bio_done = 200,
    reg_done = 202,      force_stop_done = 322, force_facematch = 323, force_nomatch = 324,   error = 0      
};


// functions declerations

void door_detection();         
void device_configure(void);
void fingerprint_enroll();
void device_connecting();
void working_mode();
void locking_system();
void fingerprint_detection();
void reset_to_device();
void device_config_mode();
void route_confirm_mode();                                ;
void panic_nroute_mode();

//  Command get_command(String data);

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
  {
    deviceConnected = true;                       // 
    Serial.println("Device connected");
    Serial.print("Device address: ");             // 
    Serial.print("RSSI: ");                       //  
  };

  ///////////////

  void onDisconnect( BLEServer *pServer) {  deviceConnected = false; }  };

 class MyCallbacks : public BLECharacteristicCallbacks 
 {
   void onWrite(BLECharacteristic *pCharacteristic)
    {
      new_string = "0";           
      for (int i = 0; i < sizeof(incomingData); i++) { incomingData[i] = '\0'; }        
      String rxValue = pCharacteristic->getValue();

      if ( rxValue.length() > 0)
      {
        indx = ( rxValue.length() );                                                     // 
        for (int j = 0; j < indx; j++)   { incomingData[j] = rxValue[j]; }               // 
        new_string = String(incomingData);                                                // 
        Serial.println( new_string );     

      }
       delay(50);
    }
};


////////////

void setup()
{
  
  //pinMode(rgb_led, OUTPUT);         
  pinMode(relay, OUTPUT);           
  pinMode(buzzer, OUTPUT);          
  pinMode(led_g, OUTPUT);          
  pinMode(led_b, OUTPUT);           
  pinMode(led_r, OUTPUT);      
  pinMode(reset_btn, INPUT);    
  pinMode(door, INPUT);        
  digitalWrite(relay, LOW);    
  digitalWrite(buzzer, LOW);    
  digitalWrite( door,  LOW );  
  digitalWrite( led_g, HIGH ); 
  digitalWrite( led_b, HIGH ); 
  digitalWrite( led_r, HIGH ); 
  digitalWrite(reset_btn, HIGH );

  Serial.begin(57600);  //  (9600);
  Serial2.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  ////////// Biometric Module /////////

  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  
  finger.begin(57600);     // set the data rate for the sensor serial port

  if (finger.verifyPassword()) { Serial.println("Found fingerprint sensor!"); }

  else { Serial.println("Did not find fingerprint sensor ");  }  //while (1) {  led_blink('r'); }  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);
  delay(100);

  // EEPROM.writeString(10, "o0000");
  // EEPROM.commit();

  device_mode = EEPROM.read(1);
  Serial.println( device_mode );    
  device_pwd  = EEPROM.readString(10);
  Serial.println( device_pwd );
  get_mac_id();
  ble_name = (ide + mac_string);
  Serial.println(ble_name);


  ////// Create the BLE Device  /////////

  BLEDevice::init(ble_name);                                       //  BLE display Name
  
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(
  CHARACTERISTIC_UUID_TX,
  BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
  CHARACTERISTIC_UUID_RX,
  BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setAppearance(BLE_APPEARANCE_GENERIC_HID);        

  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  
  btn_status = digitalRead( reset_btn );                // reset button scanner 
  if ( btn_status == 0 ) { psw_reset_mode();   }

}


//********************************


  void ble_connection()
  {
    if (!deviceConnected)                          // device not connected
    {
      
      pServer->startAdvertising();                // restart advertising
      Serial.println("Device connecting......");
      while (!deviceConnected)  {  led_blink('b');  }     
      digitalWrite( led_g, LOW);
      delay(250);
      log_print("Device connected");
      send_message_bt1(device_connect);        //message = "100";
      delay(200);
      send_mac_add();
    }
 
     else if (deviceConnected)  { log_print("Already Connected");   } 
    delay(50);
  }

//***************

void log_print(String s_data)    {  Serial.println(s_data); }
void get_command(ble_command cc) {  Serial.println(cc);  }

//**************


////////////////////////////////////////

void loop()
{
  switch (device_mode)
  {

    case 'S':                            // starting state
     device_connecting();              //  Device BLE connection establish 
    break;
   
    case 'R':                          
     working_mode();                    // config, panic, nrstart, confirm, free_ride etc
    break; 
   
    case 'T':
     reset_to_device();
    break;

    default:
      log_print("Device Mode not Match");
      log_print("Store Device Mode");
      EEPROM.write(1, 'S');
      EEPROM.commit();
      delay(50);
      delay(2000);
    break;

  }

}


//******  BLE Connection with mobile app  connected or disconnected


void device_connecting() 
{
    unsigned long c_millis = 0;
    unsigned long p_millis = 0;
    log_print("device in connecting mode");
    led_blink('c');                                    
    if ( !deviceConnected )                         // device not connected
    {
      finger_match = false;
      pServer->startAdvertising();                 // restart advertising
      log_print("Device connecting......");
      while ( !deviceConnected )                  
      { 
       c_millis = millis(); 
       if ( c_millis - p_millis >= 700 )  { p_millis = c_millis; digitalWrite( led_b, !digitalRead(led_b) ); }
        getFingerprintID();                       // Read Biometric module           
        if ( finger_match == true )
        { 
          log_print(" Fingerprint match");
          log_print(" engine control by Finger ");
          free_ride(); 
        }
        delay(500);
      }

      finger_match = false;                           
      led_blink('c');
      digitalWrite( led_g, LOW);
      log_print("Device Connected");
      send_message_bt1(device_connect);          
      delay(100);
      send_mac_add();
      device_mode = 'R'; 
    }

    else if ( deviceConnected )               // Device conneted
    { 
      digitalWrite( led_b, LOW);
      log_print("Already Connected");
      led_blink('c');
      digitalWrite( led_g, LOW);
      device_mode = 'R';
    }
    delay(50);
  }


// ************ device working mode 

void working_mode()
{
  send_message_bt("Working_mode");
  digitalWrite( led_g, LOW);
  new_string = "nill";                       // led_blink('c');   // digitalWrite( led_g, LOW);
  Serial.println("Waiting for route confirm / panicon / nrstart/ fpenroll,delte,empty / config / disconnetion / o/m psw ");
  while ( deviceConnected )
  {
    if ( new_string.equals("confirm") )                     // Route confirmation
    {
      Serial.println("Route confirmed ");  
      send_message_bt1(confirm_done);
      vehicle_start(); 
      route_confirm_mode();
    }
      
    else if ( new_string.equals("panicon") )                //  Panic button pressed
    {
      Serial.println("Device in Panic Mode Start");
      send_message_bt1(panicon_done);
      panic_nroute_mode(); 
    }
    
    else if ( new_string.equals("nrstart") )                 // No route pressed
    { 
      Serial.println("Device in Noroute// Mode Start");
      send_message_bt1(nrstart_done);
      panic_nroute_mode(); 
    } 

    else if ( new_string.equals("fpenroll") )                // Fingerprint enroll
    {
      log_print("Finger print Enroll");
      fingerprint_enroll();
    }

    else if ( new_string.equals("fpdelete") )                 // Fingerprint delete
    {
      log_print("Finger print delete");
      fingerprint_delete(); 
    }  
     
    else if ( new_string.equals("fpempty") )                  // Finger module empty
    {
      log_print("Bio module empty");
      fpmodule_empty();  
    }

    else if ( new_string[0] == 'o' )                           // Pair pasword 
    {
      if ( new_string != device_pwd )        { log_print("wrong password");   send_message_bt1( error );   new_string = "nill";  }  //  
      else if ( new_string == device_pwd  ) 
      {
       log_print("Device paired"); 
       send_message_bt1( dpsw_done );                    
       add_device();
       new_string = "nill";
      }
    }
 
    else if ( new_string[0] == 'm')                        // Set new password 
    {
      log_print("new password detected");      
        new_string.remove(0,1);
        log_print(new_string);
        new_string = ( "o" + new_string);
        log_print(new_string);
        log_print("New password ");  
        log_print(new_string);
        send_message_bt1( newpsw_done );               
        device_pwd = new_string;    
        EEPROM.writeString(10, device_pwd);
        EEPROM.commit();
        new_string = "nill";       
          
    } 
 
    else if ( new_string == "done" )                        //  done pressed and device configuration completed
    {
     send_message_bt1( reg_done );
     log_print("Device Config. done");
     log_print("Device will be restart in 1 seconds");
     delay(1000);
     esp_restart();
    }

    delay(250);       
    
  }


  if ( !deviceConnected )                             // device not connected
  {
   log_print("device Disconnected");
   //device_connecting(); 
   device_mode = 'S';
   new_string = "nill";
   loop();
  }

}
    
  
//********************  panic on and nrstart mode 

void panic_nroute_mode()            
{
  vehicle_start();
  log_print("Waiting for panicoff / nrstop ");
  
  while ( new_string != "panicoff" && new_string != "nrstop" && deviceConnected )   { delay(150);  } 

  log_print("Device out form Panic/fride");     // digitalWrite(relay, LOW);   delay(5);   digitalWrite(relay, LOW); 
  //vehicle_stop();    
  if ( new_string == "panicoff")       { log_print("panic stop");          send_message_bt1(panicoff_done);  } 
  else if ( new_string == "nrstop")    { log_print("No route stop");       send_message_bt1(nrstop_done);    }
  vehicle_stop();                      //send_message_bt1(vstop_done);
  log_print("Engine stop"); 

}


//********************************  Vehicle route confirmation


void route_confirm_mode()   
{
  door_open = false;
  //vehicle_start();            
  while (new_string != "deviate" &&  deviceConnected &&  door_open == false  &&  new_string != "destination" && new_string != "force_stop" )   // Data synchronize with mobie app while driving
  {
    delay(3000);
    send_message_bt1(ok_fine);
    door_status = digitalRead(door);                      // if ( door_status == 0) {  door_detection();   }   
    if ( door_status == 1) {  door_detection();   }
  }

  if ( new_string.equals("destination") ) 
  { 
    new_string = "nill"; 
    log_print("Route Complete"); 
    send_message_bt1( desti_done); 
    vehicle_stop();
    working_mode();   
    
  }
  else if (  new_string.equals("force_stop") ) {  forcely_stop(); }
  else if ( new_string.equals("deviate") )     {  log_print("Route deviate"); locking_system();  }       //send_message_bt1( deviate_done); }
  else if ( door_open == true )                {  log_print("Door open");     locking_system();  }     // send_message_bt1( d_open ); }
  else if ( !deviceConnected  )                {  log_print("Connection Lost / vehicle theft "); locking_system(); }
  // locking_system();                      // Go to the locking system 

}

//********************************  Door Lock detection

void door_detection()
{
  int value = 0;
  int wait = 0;
  delay(500);
  door_status = digitalRead(door);
  if (door_status == 1)                             
  {
    log_print("Door open");
    send_message_bt1( d_open );                 
    log_print("wait for 30 Second");
    while (door_status == 1 && value < 15)      
    {
      led_blink('g');  
      door_status = digitalRead(door); 
      value++;
    }

    digitalWrite( led_g, LOW );      
    delay(250);
    log_print("30 second time up ");
    door_status = digitalRead(door);          
  
    switch ( door_status )                    
    {
      case 1:                 
        finger_match = false;
        log_print("Biometric On");
        log_print("Press Finger");
        send_message_bt1(bio_scan);           
        check_time();                        

        while (wait != 1)               // Time for 1 minute
        {
          digitalWrite(buzzer, HIGH);             
          delay(200);
          digitalWrite(buzzer, LOW);             
          delay(600);                             
          check_time();
          getFingerprintIDez();                  
          if ( finger_match == true)
          {
            log_print("Fingerprint match");
            send_message_bt1( bio_match );       
            door_open = false;
            finger_match = false;
            wait = 1;
          }

          if (tick_value >= 60) 
          {
            log_print("Finger Not Press/not match");
            log_print("Time Up");
            send_message_bt1( bio_nomatch );     
            vehicle_stop();
            door_open = true;
            finger_match = false;                 
            log_print("vehicle theft");
            fingerprint_detection(); 
            wait = 1;
          }
        }

        break;

      case 0:                               // Door is close       
        door_open = false;                  
        send_message_bt1(d_close);                  

        break;
    }
  }
}


//********************************  Vehicle Engine Locking system


void locking_system()
{
  switch (door_open)
  {
    case true:

      delay(1000);
      log_print("Door close time up");
      vehicle_stop();
      fingerprint_detection();                     

      break;

    case false:

      if ( !deviceConnected )                      
      {
        log_print("Device Disconnected");
        log_print ("Vehicle Theft");              
        vehicle_stop();
        fingerprint_detection();                                         
      }

      else if ( deviceConnected )                   // route deviate   
      {
        
        send_message_bt1( deviate_done );                            
        log_print("Route deviated..");
        log_print("Waiting... for Face/sound matching");     
        
        while ( new_string != "match" && new_string != "notmatch" )  {  delay(250);   }

        if (new_string == "notmatch")
        {
          send_message_bt1( facenomatch );         
          log_print("Face/sound not match");
          vehicle_stop();
          led_blink('c');
          digitalWrite(led_r, LOW);                 
          fingerprint_detection();                    

        }
        
        else if (new_string == "match")
        {
          send_message_bt1(facematch );      
          log_print("FP,Keyw match");
          //working_mode();
          log_print("back to confirm mode");
          route_confirm_mode();                   
        }
      }

      break;
  }
}


//********************************  Fingerprint detection


void fingerprint_detection()
{
  int hold = 0;
  finger_match = false;
  log_print("Biometric On");
  log_print("Press Finger");          
  send_message_bt1( bio_scan );       
 
  while (hold != 1)  
  { 
    getFingerprintID();                
    delay( 300 );                      
    if (finger_match == true)
    {
      log_print("Fingerprint match");
      send_message_bt1(bio_match);        
      hold = 1;
    }
  }

  finger_match = false;
  one_hour_timer();          

}

//********************************

void vehicle_stop()
{
  delay(30000);                                                          
  digitalWrite(relay, LOW);  delay(5); digitalWrite(relay, LOW);          
  log_print("Engine stop");
  send_message_bt1(vstop_done);                                       
 
}

//********************************

void vehicle_start()
{
  digitalWrite(relay, HIGH);  delay(5);    digitalWrite(relay, HIGH);  
  log_print("Engine Start");
  send_message_bt1(vstart_done);
}

//********************************


void one_hour_timer() 
{
  led_blink('c');
  log_print("One hour timer on");                  
  vehicle_start();
  for (int j = 0; j < 60; j++)          // 60
  {
    for ( int k = 0; k < 30; k++ )  {   led_blink('g');  }                             
  }
  led_blink('c');
  vehicle_stop();  
  log_print("one hour timer OFF"); 
  device_mode = 'T';                       
  EEPROM.write(1, device_mode);           
  EEPROM.commit();
  log_print("device mode T");
  reset_to_device();                      

}


//********************



uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}


//********************


void check_time()
{
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    tick_value++;
  }
}


//********************* Reset the device


void reset_to_device() 
{
  int stay = 0;
  new_string = "nill";
  log_print("Reset the Device");
  delay(2000);
  while ( new_string != "reset" )
  {
    if ( deviceConnected)  {  log_print("Already Connected");  }

    else if (!deviceConnected)                          
    {
      pServer->startAdvertising();                         
      log_print("Device connecting......");

      while ( !deviceConnected )  {  led_blink('r');  }
      digitalWrite(led_r, LOW);                          
      log_print("Device connected");
      send_message_bt1( device_connect );            
      delay(200);
      send_mac_add();
    }
    
    log_print("Device in Reset Mode");
    send_message_bt1( reset_mode );             

    while ( new_string != "reset" && deviceConnected )  {  delay( 250 );  }  
    
  }

  digitalWrite(buzzer, HIGH);                  
  delay(1000);
  digitalWrite(buzzer, LOW);                   
  log_print("Device Reset Ok");
  send_message_bt1( reset_done );                
  new_string = "nill";
  led_blink('c');
  device_mode = 'S';
  EEPROM.write(1, device_mode); 
  EEPROM.commit();
  delay(1000);  
  loop();
}


//********************************


void buz_beep()
{
  for (int k = 0; k < 5; k++)
  {
    digitalWrite(buzzer, HIGH);  
    delay(50);                   
    digitalWrite(buzzer, LOW);  
    delay(50);
  }
}



//*********************** Add the device during Registrtion

void add_device()
{
  get_mac_id();
  send_mac_add();
  delay(200);         
  if (new_string == "1") { log_print("Device Add successfully"); }
  else if (new_string == "0") { log_print( "Error device not addedd" ); }
  new_string = "nill";                     
}

//********************************

void led_blink(char led_color)
{
  switch (led_color)
  {
      case 'g':
      digitalWrite(led_g, LOW);  delay(1000);
      digitalWrite(led_g, HIGH); delay(1000);
      break;

      case 'b':
      digitalWrite(led_b, LOW);  delay(1000);
      digitalWrite(led_b, HIGH); delay(1000);
      break;

      case 'r':
      digitalWrite(led_r, LOW);  delay(1000);
      digitalWrite(led_r, HIGH); delay(1000);
      break;

      case 'c':
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_b, HIGH);
      digitalWrite(led_r, HIGH); 
      break;

  }

}

//************ Free ride with biometric module 

void free_ride()
{
  led_blink('c');
  log_print("Free ride on ");                   // send_message_bt1( vstart_done );     //  message = "313";  //"Engine Start for one hour";
  vehicle_start();
  for (int j = 0; j < 60; j++)       // 60
  {
    for ( int k = 0; k < 30; k++ )  {   led_blink('g');  }      // one minute timer                            
  }
  led_blink('c');
  vehicle_stop();  
  log_print("Free ride off "); 
  device_connecting(); 
}


//************ Password Reset function

void psw_reset_mode()
{
 delay(3000);
 btn_status = digitalRead( reset_btn );
 if ( btn_status == 0 ) 
  {
    log_print("Password reset mode ON ");
    EEPROM.writeString(10, "o0000");
    EEPROM.commit();
    buz_beep();
    log_print("Password reset done "); 
    delay(1000);
    esp_restart();                   // restart the device 
  }

  else if ( btn_status == 1 ) { log_print("Password not rest error ");  }
  
}


//********************************

void get_mac_id() 
{
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8)  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  mac_string = String(chipId);
  log_print(mac_string);

}



//**********************

void send_mac_add()
 {
  log_print("MAC ID: ");
  log_print(ide + mac_string);                     
  pTxCharacteristic->setValue(ide + mac_string);  
  pTxCharacteristic->notify();
  delay(150);
}



//*****************


void send_message_bt(String mssg)
{
  pTxCharacteristic->setValue(mssg);
  pTxCharacteristic->notify();
  delay(100);
}  


//********************

void send_message_bt1( ble_command ble_data )
{
  String ble_cmd = String(ble_data);
  pTxCharacteristic->setValue(ble_cmd);
  pTxCharacteristic->notify();
  log_print(ble_cmd);
  delay(200);
}

//**********************

void forcely_stop()
{
  log_print("device forcely stop");
  send_message_bt1( force_stop_done );
  while(  new_string != "match" && new_string != "notmatch" ) {  buz_beep();  delay(1000); }
  if ( new_string == "match")
    {
      send_message_bt1( force_facematch );      
      log_print("face match in forcely stop mode");
      vehicle_stop();
      working_mode();
                         
    }

  else if (new_string == "notmatch" )
  {
      send_message_bt1( force_nomatch );         
      log_print("face no-match in forcely stop mode");
      new_string = "nill"; 
      log_print("back to route confirmation mode");
      route_confirm_mode();      

  }
  

  
}

/*****************///////////////////////*************************//


