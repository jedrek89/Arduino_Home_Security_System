// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial sim800l(16, 17); // TX, RX, YELLOW, PURPLE.
//SoftwareSerial mySerial(18, 19);
#define mySerial Serial1 // CZYTNIK LP
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
short int fingerprint_ID;
int PIR_1 = 30; // CZUJNIK PIR_1
int LED_1 = 31; // LED_1
void data_czas(); // Funkcja wyswietlanie daty i czasu
void spr_pin();
char key;
//void czuwanie_zal();
//NUMERY TEL DO POWIADOMIEŃ
String nr_tel_1 = "+48721257798";
// Stały pin
static char pin_s1 = '7';
static char pin_s2 = '5';
static char pin_s3 = '8';
static char pin_s4 = '7';
int menu = 0;

int akt_min;  // POBRANIE AKTUALNEJ MINUTY Z RTC
int poz_dost = 0; // 0 - NIEPOPRAWNE    4 - OK
int poz_wsk = 0;
int stan_czuwania = 0; // 0 - CZUWANIE WYŁ, 2 CZUWANIE WŁ.


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
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));// MANUALNE USTAWIENIE CZASU 

    // WYŚWIETLENIE EKRANU POWITALNEGO
   lcd.setCursor(0, 1);
   lcd.print ("SYSTEM ALARMOWY     ");
   lcd.setCursor(0, 2);
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

  if (stan_czuwania == 1)
    {
      czuwanie_zal();
    }

////////////////////////////////////////////////////////// 
  if (key == 'A' || menu == 10 && stan_czuwania == 0)
      {
        menu = 10;
        lcd.setCursor(0, 1);
        lcd.print ("TRYB AKTYWACJI      "); 
        key_pad();
        getFingerprintIDez();
        Serial.println("Odcisk:");
        Serial.println(fingerprint_ID);
        //delay (50);
        if (fingerprint_ID == 1 || fingerprint_ID == 2 || fingerprint_ID == 3 || fingerprint_ID == 4 )
          {
            lcd.setCursor(0, 1);
            lcd.print ("CZUWANIE ZAL        ");
            delay (1000);
            stan_czuwania = 1;
            poz_dost = 0;
            lcd.setCursor(5, 2);
            lcd.print ("              "); 
            fingerprint_ID = 0;
            menu = 0; 
          }
        if (key && poz_dost !=4 && menu == 10)
            {
                spr_pin();
                  if (poz_dost == 4)
                    {
                      lcd.setCursor(0, 1);
                      lcd.print ("CZUWANIE ZAL        ");
                      delay (1000);
                      stan_czuwania = 1;
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
        delay (50);
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
        if (key && poz_dost !=4 && menu == 10)
            {
                spr_pin();
                  if (poz_dost == 4)
                    {
                      lcd.setCursor(0, 1);
                      lcd.print ("CZUWANIE ZAL        ");
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
        lcd.print ("OPCJE <> * #        ");
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

         
         //stan_czuwania = 1; // zalaczenie czuwania       
}

//////////////////////////////////////////////////////////
//  FUNKCJA ODLICZANIE
//////////////////////////////////////////////////////////
int odliczanie()
{ 
  DateTime now = rtc.now();
  akt_min = now.minute(); 
  lcd.setCursor(0, 3);
  lcd.print(akt_min);          

      
         
}

//////////////////////////////////////////////////////////
//  FUNKCJA SPRAWDZĄCA POPRAWNOŚĆ PINU
//////////////////////////////////////////////////////////

void spr_pin()
{ 
      if (poz_wsk == 0 && key == pin_s1 && poz_dost == 0)
      { 
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu
         poz_dost=1;
         poz_wsk++;  
      }
      else if (poz_wsk == 1 && key == pin_s2 && poz_dost == 1)
      {
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu
         poz_dost=2;
         poz_wsk++;  
      }
      else if (poz_wsk == 2 && key == pin_s3 && poz_dost == 2)
      {
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         Serial.println("Poziom dotepu: "), Serial.println (poz_dost);  // Kontrola poziomu dostępu 
         poz_dost=3;
         poz_wsk++;   
      }
      else if (poz_wsk == 3 && key == pin_s4 && poz_dost == 3)
      {
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         poz_dost=4;
         Serial.println("Poziom dotepu: ");
         Serial.println (poz_dost);  // Kontrola poziomu dostępu
         lcd.setCursor(0, 1);
         lcd.print ("PIN OK!             ");
         delay(2000);
         lcd.setCursor(0, 1);
         lcd.print ("                    ");
         lcd.setCursor(5, 2);
         lcd.print ("              "); 
         poz_wsk++;
         poz_wsk = 0;  
      }
      
      else
      { 
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         lcd.setCursor(0, 3);
         lcd.print (poz_dost);
         poz_wsk++;
         if (poz_wsk >= 4 && poz_dost != 4)
         {
            lcd.setCursor(0, 1);
            lcd.print ("ZLY PIN !           ");
            delay(2000);
            //lcd.setCursor(0, 1);
            //lcd.print ("                    ");
            //delay(200); 
            lcd.setCursor(0, 1);
            lcd.print ("WPROWADZ PIN        ");
            lcd.setCursor(5, 2);
            lcd.print ("               ");
            delay(1000); 
            lcd.setCursor(0, 3);    // DO USUNIĘCIA 
            lcd.print (poz_dost);   // DO USUNIĘCIA
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
    lcd.setCursor(0, 0);
    //lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    lcd.print(now.day());
    lcd.print('/');
    lcd.print(now.month());
    lcd.print('/');
    lcd.print(now.year());
    // Wyswietlanie godziny
    lcd.setCursor(11, 0);    
    if (now.hour()<10)
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
    if (now.minute()<10)
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
    // Wyswietlanie minuty
    lcd.setCursor(17, 0);
    // Wyswietlanie sekundy
     if (now.second()<10)
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