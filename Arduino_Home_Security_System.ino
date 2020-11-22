// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <LiquidCrystal.h>
#include "RTClib.h"
#include <Keypad.h>

char key;
void data_czas(); // Funkcja wyswietlanie daty i czasu
void spr_pin();
// Stały pin
static char pin_s1 = '7';
static char pin_s2 = '5';
static char pin_s3 = '8';
static char pin_s4 = '7';

int poz_dost = 0; // 0 - NIEPOPRAWNE    4 - OK
int poz_wsk = 0;
int stan_alarmu = 0; // Domyślny stan alarmu


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
  Serial.begin(9600);
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
   lcd.print ("System Alarmowy     ");
   lcd.setCursor(0, 2);
   lcd.print ("PIN: ");
   
}
///////////////////////LOOP/////////////////////////////// 
void loop () 
{
   data_czas(); // Wywolanie funkcji wyświetlania czasu rzeczywistego
   key = keypad.getKey();
    if (key && poz_dost != 4) 
    {
      spr_pin();
    }


   delay(50);
   
} 
//////////////////////END LOOP////////////////////////////


//FUNKCJA SPRAWDZĄCA POPRAWNOŚĆ PINU
void spr_pin()
{ 
      if (poz_wsk == 0 && key == pin_s1 && poz_dost == 0)
      { 
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         lcd.setCursor(0, 3);
         poz_dost++;
         lcd.print (poz_dost);
         poz_wsk++;  
      }
      else if (poz_wsk == 1 && key == pin_s2 && poz_dost == 1)
      {
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         lcd.setCursor(0, 3);
         poz_dost++;
         lcd.print (poz_dost);
         poz_wsk++;  
      }
      else if (poz_wsk == 2 && key == pin_s3 && poz_dost == 2)
      {
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         lcd.setCursor(0, 3);
         poz_dost++;
         lcd.print (poz_dost);
         poz_wsk++;   
      }
      else if (poz_wsk == 3 && key == pin_s4 && poz_dost == 3)
      {
         lcd.setCursor(5 + poz_wsk, 2);
         lcd.print('*');
         lcd.setCursor(0, 3);
         poz_dost++;
         lcd.setCursor(0, 1);
         lcd.print ("Pin ok!              ");
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
            lcd.print ("Zly pin !     ");
            delay(2000);
            lcd.setCursor(0, 1);
            lcd.print ("                     "); 
            lcd.setCursor(5, 2);
            lcd.print ("                ");
            lcd.setCursor(0, 3); 
            lcd.print (poz_dost);
            //poz_wsk++;
            poz_wsk = 0;
            poz_dost = 0;   
         }
         }    
      
   
}
//////////////////////////////////////////////////////////

//FUNKCJA WYSWIETLAJĄCA AKTUALNĄ DATĘ I CZAS
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
