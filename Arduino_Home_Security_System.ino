// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial sim800l(16, 17); // TX, RX, YELLOW, PURPLE.
//SoftwareSerial mySerial(18, 19);
#define mySerial Serial1 // FINGERPTINT READER
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
short int fingerprint_ID;
int PIR_1 = 30; //  PIR_1 SENSOR
int LED_1 = 31; //  LED_1
void data_czas(); //  GET CURRENT TIME FROM RTC
void spr_pin();
char key;
//void czuwanie_zal();
//NUMERY TEL DO POWIADOMIEŃ
String nr_tel_1 = "*************";
// Stały pin
static char pin_s1 = '7';
static char pin_s2 = '5';
static char pin_s3 = '8';
static char pin_s4 = '7';
int menu = 0; 
int czas_akt = 60; // DELAY TO ARM ALARM
int akt_sec = 60;  // GET CURRENT SEC FROM RTC
int stan_odlicz = 0;  // STATE OF COUNTING
int poz_dost = 0; // 0 - NOK    4 - OK
int poz_wsk = 0;
int stan_czuwania = 0;  // 0 - CZUWANIE WYŁ, 2 CZUWANIE WŁ.


RTC_DS1307 rtc;
LiquidCrystal lcd(11, 10, 9, 8, 7, 6); // (rs, e, d4, d5, d6, d7)

//char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const byte ROWS = 4;
const byte COLS = 4;
char keys [ROWS][COLS]={
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
  };

byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup () 
{ 
  sim800l.begin(9600);
  Serial.begin(9600);
  finger.begin(57600);
  
  pinMode(PIR_1, INPUT);  // CZUJNIK PIR_1 
  pinMode(LED_1, OUTPUT); // LED_1
  digitalWrite(PIR_1, LOW);

  
  lcd.begin(20, 4);
  if (! rtc.begin()) 
  {
    lcd.print("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) 
  {
    lcd.print("RTC is NOT running!");
  }
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));// USTAWIENIE CZASU Z KOMPUTERA
    //rtc.adjust(DateTime(2020, 12, 12, 3, 0, 0));// MANUALNE USTAWIENIE CZASU 

    // WYŚWIETLENIE EKRANU POWITALNEGO
   lcd.setCursor(0, 1);
   lcd.print ("SYSTEM ALARMOWY     ");
   lcd.setCursor(0, 3);
   lcd.print ("PIN: ");

   finger.getTemplateCount();
   Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
   Serial.println("Waiting for valid finger...");
   
}
////////////////////////////////////////////////////////// 
//  LOOP 
////////////////////////////////////////////////////////// 
void loop () 
{
   data_czas(); // Wywolanie funkcji wyświetlania czasu rzeczywistego
   //getFingerprintIDez(); // Pobranie odcisku palca
   Serial.println("menu:");Serial.println(menu);
   
  if (menu == 0)
    {
      key_pad();  
    }

  if (stan_czuwania == 1 && stan_odlicz == 0)
    {
      odliczanie();
      lcd.setCursor(16, 1);
      delay (50);
      lcd.println(akt_sec);
      lcd.setCursor(18, 1);
      lcd.println("  ");

      Serial.println("ODLICZANIE RTC"), Serial.println (akt_sec); 
    }

  if (stan_czuwania == 1 && stan_odlicz == 1)
    {
      czuwanie_zal();
    }

////////////////////////////////////////////////////////// 
  if (key == 'A' || menu == 10 && stan_czuwania == 0)
      {
        menu = 10;
        lcd.setCursor(0, 1);
        lcd.print ("TRYB AKTYWACJI     o"); 
        key_pad();
        getFingerprintIDez();
        Serial.println("Odcisk:");
        Serial.println(fingerprint_ID);
        if (fingerprint_ID == 1 || fingerprint_ID == 2 || fingerprint_ID == 3 || fingerprint_ID == 4 )
          {
            lcd.setCursor(0, 1);              
            lcd.print ("CZUWANIE ZAL ZA:   o");
            delay (1000);
            stan_czuwania = 1;
            stan_odlicz = 0;
            poz_dost = 0;
            //lcd.setCursor(5, 2);
            //lcd.print ("              "); 
            fingerprint_ID = 0;
            menu = 0; 
          }
        if (key && poz_dost !=4 && menu == 10)
            {
                spr_pin();
                  if (poz_dost == 4)
                    {
                      lcd.setCursor(0, 1);
                      lcd.print ("CZUWANIE ZAL ZA: ");
                      delay (1000);
                      stan_czuwania = 1;
                      stan_odlicz = 0;
                      poz_dost = 0;
                      menu = 0;  
                    }
             }
           
      }
      
////////////////////////////////////////////////////////// 
  if (key == 'D' || menu == 20 && stan_czuwania == 1)
      {
        menu = 20;
        lcd.setCursor(0, 1);
        lcd.print ("TRYB DEZAKTYWACJI   "); 
        key_pad();
        getFingerprintIDez();
        Serial.println("Odcisk:");
        Serial.println(fingerprint_ID);
        if (fingerprint_ID == 1 || fingerprint_ID == 2 || fingerprint_ID == 3 || fingerprint_ID == 4 )
          {
            lcd.setCursor(0, 1);
            lcd.print ("CZUWANIE WYL        ");
            delay (1000);
            stan_czuwania = 0;
            poz_dost = 0;
            lcd.setCursor(5, 2);
            lcd.print ("              "); 
            fingerprint_ID = 0;
            menu = 0; 
          }
        if (key && poz_dost !=4 && menu == 20)
            {
                spr_pin();
                  if (poz_dost == 4)
                    {
                      lcd.setCursor(0, 1);
                      lcd.print ("CZUWANIE WYL        ");
                      delay (1000);
                      stan_czuwania = 0;
                      poz_dost = 0;
                      menu = 0;  
                    }
             }
           
      }

////////////////////////////////////////////////////////// 
  if (key == 'B' || menu == 30)
      {
        menu = 30;
        lcd.setCursor(0, 1);
        lcd.print ("(<>*#)(OK-0)(D-BACK)");
        lcd.setCursor(0, 2);
        lcd.print ("USTAWIENIA           ");

        Serial.println(menu);         
      }
           
   delay(100);
} 

//////////////////////////////////////////////////////////
//  END LOOP
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// KEYPAD
//////////////////////////////////////////////////////////
void key_pad()
  {
      key = keypad.getKey();
      Serial.println(key);
      delay(100);   
  }

//////////////////////////////////////////////////////////
// FUNKCJA CZUWANIE
//////////////////////////////////////////////////////////
void czuwanie_zal()
{        

  if (digitalRead(PIR_1) == HIGH)
     {
        digitalWrite(LED_1, HIGH);
     }
          
  else
     {
        digitalWrite(LED_1, LOW);  
     }         
     
}
//////////////////////////////////////////////////////////
//  FUNKCJA ODLICZANIE
//////////////////////////////////////////////////////////
int odliczanie()
{ 
  DateTime now = rtc.now();
  akt_sec = czas_akt - now.second();
  delay(20);
  if (akt_sec == 0)
    {
      stan_odlicz = 1; 
    }
  //Serial.println("ODLICZANIE RTC"), Serial.println (akt_sec); 
  
  return akt_sec;
}

//////////////////////////////////////////////////////////
//  FUNKCJA SPRAWDZĄCA POPRAWNOŚĆ PINU
//////////////////////////////////////////////////////////

void spr_pin()
{ 
      if (poz_wsk == 0 && key == pin_s1 && poz_dost == 0)
      { 
         lcd.setCursor(5 + poz_wsk, 3);
         lcd.print('*');
         //Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu
         poz_dost=1;
         poz_wsk++;  
      }
      else if (poz_wsk == 1 && key == pin_s2 && poz_dost == 1)
      {
         lcd.setCursor(5 + poz_wsk, 3);
         lcd.print('*');
         //Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu
         poz_dost=2;
         poz_wsk++;  
      }
      else if (poz_wsk == 2 && key == pin_s3 && poz_dost == 2)
      {
         lcd.setCursor(5 + poz_wsk, 3);
         lcd.print('*');
         //Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu 
         poz_dost=3;
         poz_wsk++;   
      }
      else if (poz_wsk == 3 && key == pin_s4 && poz_dost == 3)
      {
         lcd.setCursor(5 + poz_wsk, 3);
         lcd.print('*');
         poz_dost=4;
          //Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu 
         lcd.setCursor(0, 1);
         lcd.print ("PIN OK!            o");
         delay(2000);
         lcd.setCursor(0, 1);
         lcd.print ("                   o");
         lcd.setCursor(5, 3);
         lcd.print ("              o"); 
         poz_wsk++;
         poz_wsk = 0;  
      }
      
      else
      { 
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
          //Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu 
         poz_wsk++;
         if (poz_wsk >= 4 && poz_dost != 4)
         {
            lcd.setCursor(0, 1);
            lcd.print ("ZLY PIN !          o");
            delay(2000);
            lcd.setCursor(0, 1);
            lcd.print ("WPROWADZ PIN       o");
            lcd.setCursor(5, 3);
            lcd.print ("              o");
            delay(1000); 
            poz_wsk++;
            poz_wsk = 0;
            poz_dost = 0;   
         }
         }    
    
}
//////////////////////////////////////////////////////////
//  RTC MODULE - SHOWING CURENT DATE AND TIME ON LCD
//////////////////////////////////////////////////////////
  void data_czas()
  {
    DateTime now = rtc.now();
    // Wyswietlanie daty i dnia tygodnia  
    //lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // Wyswietlanie dnia
    lcd.setCursor(0, 0);
    if (now.day() < 10)
      {
        lcd.print('0');
        lcd.setCursor(1, 0);
        lcd.print(now.day());
        lcd.setCursor(2, 0);
      }
    else
      {
        lcd.print(now.day());
      }
      lcd.print('/');
    //lcd.print(now.day());
    //lcd.print('/');
    // Wyswietlanie miesiąca
    lcd.setCursor(3, 0);
    if (now.month() < 10)
      {
        lcd.print('0');
        lcd.setCursor(4 , 0);
        lcd.print(now.month());
      }
    else
      {
        lcd.print(now.month());
      }
    lcd.print('/');
    lcd.print(now.year());
    // Wyswietlanie godziny
    lcd.setCursor(11, 0);    
    if (now.hour() < 10)
      {
        lcd.print('0');
        lcd.setCursor(12, 0);
        lcd.print(now.hour());
        lcd.setCursor(13, 0);        
      }
    else
      {
        lcd.print(now.hour());
        lcd.setCursor(13, 0);
      }
    lcd.print(':');
    // Wyswietlanie minuty
    lcd.setCursor(14, 0);
    if (now.minute() < 10)
      {
        lcd.print('0');
        lcd.setCursor(15, 0);
        lcd.print(now.minute());
        lcd.setCursor(16, 0);        
      }
    else
      {
        lcd.print(now.minute());
        lcd.setCursor(16, 0);
      }
    lcd.print(':');
        // Wyswietlanie sekundy
    lcd.setCursor(17, 0);
    if (now.second() < 10)
      {
        lcd.print('0');
        lcd.setCursor(18, 0);
        lcd.print(now.second());
        lcd.setCursor(19, 0);        
      }
    else
      {
        lcd.print(now.second());
        lcd.setCursor(19, 0);
      }
  }  

//////////////////////////////////////////////////////////
//  FUNKCJA SPRAWDZAJĄCA ODCISK PALCA
//////////////////////////////////////////////////////////

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  //Serial.print("Found ID #"); Serial.print(finger.fingerID);
  //Serial.print(" with confidence of "); Serial.println(finger.confidence);

switch(finger.fingerID)
{
  case 0:
  
    fingerprint_ID = 1;
    break;
  

  case 1:
  
    fingerprint_ID = 2;
    break;
  
  case 2:
  
    fingerprint_ID = 3;
    break;
  
  case 3:
  
    fingerprint_ID = 4;
    break;

  default:
  
    fingerprint_ID = 0;
    break;
  
}
  Serial.print(fingerprint_ID);
  return finger.fingerID;
  
}

//////////////////////////////////////////////////////////
//  WYSYŁANIE POWIADOMIEŃ SMS
//////////////////////////////////////////////////////////

void SendTextMessage()
{
  Serial.println("Sending Text...");
  sim800l.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  //sim800l.print("AT+CMGS=\"+48*********\"\r"); // Numer telefonu do powiadomień  
  sim800l.print("AT+CMGS=\"+48*********\"\r"); // Numer telefonu do powiadomień  
  delay(200);
  sim800l.print("Motion detected !"); //the content of the message
  delay(500);
  sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);

}