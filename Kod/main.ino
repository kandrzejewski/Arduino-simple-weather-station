//----------------------------------------------------------------------
//                DEKLARACJA  BIBLIOTEK
//----------------------------------------------------------------------
#include <Wire.h>
#include <LiquidCrystal.h>
#include "Timers.h"
#include "DS1307.h"
#include "DHT.h"


//----------------------------------------------------------------------
//                DEKLARACJA ZMIENNYCH
//----------------------------------------------------------------------
//ZMIENNE POMOCNICZE
  byte przycisk_plus = 11;
  byte przycisk_minus = 10;
  byte przycisk_tryb = 9;
  byte ustaw = 0;
  byte licznik;
  byte temperatura = 0;
  byte wilgotnosc = 0;
  char przelacz_lcd = 0;
  bool przelacz_tryb = 0;
  bool blokada;
  bool mrugniecie;
//CZUJNIK TEMPERATURY I WILGOTNOŚCI
  #define DHT_PIN 13
  DHT DHT_sensor;
//MODUŁ CZASU RZECZYWISTEGO
  DS1307 zegar;
  RTCDateTime czas;
  int RRRR = 2000;
  byte MM = 1;
  byte DD = 1;
  char GODZ = 0;
  char MIN = 0;
//EKRAN LCD
  LiquidCrystal lcd(2,3,4,5,6,7);
//TIMERY
  Timer timer_bounce;
  Timer timer_czas;
  Timer timer_temperatura;
  Timer timer_wilgotnosc;
  Timer timer_dzien;
  Timer timer_mrugniecie;
//ZNAKI WŁASNE
byte stopnie[8] = {
  0b01100, 0b11110, 0b10010, 0b11110, 0b01100, 0b00000, 0b00000, 0b00000
};
byte gora_lewy[8] = {
  0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10000, 0b10000, 0b10000
};
byte gora_srodek_czas[8] = {
  0b11111, 0b00000, 0b00000, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100
};
byte gora_prawy[8] = {
  0b10000, 0b01000, 0b00100, 0b00010, 0b00001, 0b00001, 0b00001, 0b00001
};
byte dol_lewy[8] = {
  0b10000, 0b10000, 0b10000, 0b10000, 0b01000, 0b00100, 0b00010, 0b00001
};
byte dol_srodek_czas[8] = {
  0b00010, 0b00001, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111
};
byte dol_prawy_czas[8] = {
  0b00001, 0b00001, 0b00001, 0b10001, 0b00010, 0b00100, 0b01000, 0b10000
};
byte dol_prawy_temperatura[8] = {
  0b00001, 0b00001, 0b00001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000
};
byte gora_srodek_temperatura[8] = {
  0b11111, 0b00000, 0b00000, 0b00100, 0b01010, 0b01010, 0b01010, 0b01010
};
byte dol_srodek_temperatura[8] = {
  0b01010, 0b10001, 0b11111, 0b01110, 0b00000, 0b00000, 0b00000, 0b11111,
};
byte gora_lewy_wilgotnosc[8] = {
  0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10000, 0b10000, 0b10001
};
byte gora_srodek_wilgotnosc[8] = {
  0b11111, 0b00000, 0b00000, 0b00100, 0b01010, 0b10001, 0b00000, 0b00000
};
byte gora_prawy_wilgotnosc[8] = {
  0b10000, 0b01000, 0b00100, 0b00010, 0b00001, 0b00001, 0b00001, 0b10001
};
byte dol_lewy_wilgotnosc[8] = {
  0b10001, 0b10001, 0b10000, 0b10000, 0b01000, 0b00100, 0b00010, 0b00001
};
byte dol_srodek_wilgotnosc[8] = {
  0b00000, 0b00000, 0b00000, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111
};
byte dol_prawy_wilgotnosc[8] = {
  0b10001, 0b10001, 0b00001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000
};


//----------------------------------------------------------------------
//                SETUP
//----------------------------------------------------------------------
void setup() 
{   
   //URUCHOMIENIE MAGISTRALI I2C
    Wire.begin();
   //INICJACJA PINÓw PRZYCISKÓW
    pinMode(przycisk_plus,INPUT_PULLUP);
    pinMode(przycisk_minus,INPUT_PULLUP);
    pinMode(przycisk_tryb,INPUT_PULLUP);
   //INICJACJA CZUJNIKA DHT11
    DHT_sensor.setup(DHT_PIN);
   //INICJACJA ZEGARA I USTAWIANIE CZASU KOMPILACJI
    zegar.begin();
    if (!zegar.isReady())
    {
      zegar.setDateTime(__DATE__,__TIME__);
    }
   //INICJACJA WYŚWIETLACZA LCD
    lcd.begin(16,2);
   //USTAWIANIE TIMERÓW
    timer_bounce.begin(SECS(0.3));
    timer_czas.begin(SECS(1));
    timer_temperatura.begin(SECS(1));
    timer_wilgotnosc.begin(SECS(1));
    timer_dzien.begin(SECS(1));
    timer_mrugniecie.begin(SECS(0.3));
   //TWORZENIE ZNAKU STOPNIA
    lcd.createChar(0, stopnie);
}


//----------------------------------------------------------------------
//                TWORZENIE ZNAKÓW WŁASNYCH
//----------------------------------------------------------------------
void tworzenie_znakow()
{
  if ((przelacz_lcd == 0) || (przelacz_lcd == 1))
  {
    lcd.createChar(1, gora_lewy);
    lcd.createChar(2, gora_srodek_czas);
    lcd.createChar(3, gora_prawy);
    lcd.createChar(4, dol_lewy);
    lcd.createChar(5, dol_srodek_czas);
    lcd.createChar(6, dol_prawy_czas);
  }
  if (przelacz_lcd == 2)
  {
    lcd.createChar(1, gora_lewy);
    lcd.createChar(2, gora_srodek_temperatura);
    lcd.createChar(3, gora_prawy);
    lcd.createChar(4, dol_lewy);
    lcd.createChar(5, dol_srodek_temperatura);
    lcd.createChar(6, dol_prawy_temperatura);
  }
  if (przelacz_lcd == 3)
  {
    lcd.createChar(1, gora_lewy_wilgotnosc);
    lcd.createChar(2, gora_srodek_wilgotnosc);
    lcd.createChar(3, gora_prawy_wilgotnosc);
    lcd.createChar(4, dol_lewy_wilgotnosc);
    lcd.createChar(5, dol_srodek_wilgotnosc);
    lcd.createChar(6, dol_prawy_wilgotnosc);
  }
}


//----------------------------------------------------------------------
//                ODCZYT I WYŚWIETLANIE DATY I GODZINY
//----------------------------------------------------------------------
void RTC_czas()
{
 //ODCZYTYWANIE CZASU Z MODUŁU
  czas = zegar.getDateTime();
 //WYPISANIE WARTOŚCI NA WYŚWIATLACZU
  if (timer_czas.available()) 
  {
    tworzenie_znakow();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print((char)1);
    lcd.print((char)2);
    lcd.print((char)3);
    lcd.setCursor(4,0);
    lcd.print(zegar.dateFormat("d-m-Y", czas));
    lcd.setCursor(0,1);
    lcd.print((char)4);
    lcd.print((char)5);
    lcd.print((char)6);
    lcd.setCursor(4,1);
    lcd.print(zegar.dateFormat("H:i:s", czas));
    timer_czas.restart();
  }
}


//----------------------------------------------------------------------
//                ODCZYT I WYŚWIETLANIE 
//----------------------------------------------------------------------
void RTC_dzien()
{
 //ODCZYTYWANIE CZASU Z MODUŁU
  czas = zegar.getDateTime();
 //WYPISANIE WARTOŚCI NA WYŚWIATLACZU
  if (timer_dzien.available()) 
  {
    tworzenie_znakow();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print((char)1);
    lcd.print((char)2);
    lcd.print((char)3);
    lcd.setCursor(4,0);
    lcd.print(zegar.dateFormat("l", czas));
    lcd.setCursor(0,1);
    lcd.print((char)4);
    lcd.print((char)5);
    lcd.print((char)6);
    lcd.setCursor(4,1);
    lcd.print(zegar.dateFormat("H:i:s", czas));
    timer_dzien.restart();
  }
}


//----------------------------------------------------------------------
//                ODCZYT I WYŚWIETLANIE TEMPERATURY
//----------------------------------------------------------------------
void DHT_SENSOR_temperatura()
{
 //ODCZYTYWANIE TEMPERATURY
  temperatura = DHT_sensor.getTemperature();
 //WYPISANIE WARTOŚCI NA WYŚWIETLACZU JEŻELI POPRAWNIE ODCZYTANO POMIAR
  if ((DHT_sensor.getStatusString() == "OK") && (timer_temperatura.available())) 
  {
    tworzenie_znakow();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print((char)1);
    lcd.print((char)2);
    lcd.print((char)3);
    lcd.setCursor(4,0);
    lcd.print("Temperatura:");
    lcd.setCursor(0,1);
    lcd.print((char)4);
    lcd.print((char)5);
    lcd.print((char)6);
    lcd.setCursor(4,1);
    lcd.print(temperatura);
    lcd.print((char)0);
    lcd.print("C");
    timer_temperatura.restart();
   }
}


//----------------------------------------------------------------------
//                ODCZYT I WYŚWIETLANIE WILGOTNOŚCI
//----------------------------------------------------------------------
void DHT_SENSOR_wilgotnosc()
{
 //ODCZYTYWANIE WILGOTNOŚCI
  wilgotnosc = DHT_sensor.getHumidity();  
 //WYPISANIE WARTOŚCI NA WYŚWIETLACZU JEŻELI POPRAWNIE ODCZYTANO POMIAR
  if ((DHT_sensor.getStatusString() == "OK") && (timer_wilgotnosc.available())) 
  {
    tworzenie_znakow();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print((char)1);
    lcd.print((char)2);
    lcd.print((char)3);
    lcd.setCursor(4,0);
    lcd.print("Wilgotnosc:");
    lcd.setCursor(0,1);
    lcd.print((char)4);
    lcd.print((char)5);
    lcd.print((char)6);
    lcd.setCursor(4,1);
    lcd.print(wilgotnosc);
    lcd.print("%RH");
    timer_wilgotnosc.restart();
   }
}


//----------------------------------------------------------------------
//               ZABEZPIECZENIE PRZED WYKROCZENIEM POZA ZAKRES
//----------------------------------------------------------------------
int zakres_zmiennej(int argument, int zakres_dol, int zakres_gora)
{
  if (argument == zakres_gora + 1)
  {
    argument = zakres_dol;
  }
  if (argument == zakres_dol - 1)
  {
    argument = zakres_gora;
  }
  return argument;
}


//----------------------------------------------------------------------
//                OBSŁUGA PRZYCISKÓW PLUS I MINUS
//----------------------------------------------------------------------
int przyciski_plus_minus(int argument)
{
  if ((digitalRead(przycisk_plus) == LOW) && (timer_bounce.available()))
  {
    argument = argument + 1;
    timer_bounce.restart();
  }
  if ((digitalRead(przycisk_minus) == LOW) && (timer_bounce.available()))
  {
    argument = argument - 1;
    timer_bounce.restart();
  }
  return argument;
}


//----------------------------------------------------------------------
//                FUNKCJE PRZYCISKÓW W TRYBIE WYŚWIETLANIA
//----------------------------------------------------------------------
void tryb_wyswietlania()
{
 //WYWOŁANIE FUNKCJI OBSŁUGI PRZYCISKÓW PLUS I MINUS 
  przelacz_lcd = przyciski_plus_minus(przelacz_lcd);
 //MENU WYŚWIETLANIA DANYCH NA LCD
  switch (przelacz_lcd) 
  {
    case 0 : 
    {
      RTC_czas();
      break;
    }
    case 1 :
    {
      RTC_dzien();
      break;
    }
    case 2 : 
    {
      DHT_SENSOR_temperatura();
      break;  
    }
    case 3 :
    {
      DHT_SENSOR_wilgotnosc();
      break;
    } 
  }
 //ZABEZPIECZENIE PRZED WYKROCZENIEM POZA ZAKRES
  przelacz_lcd = zakres_zmiennej(przelacz_lcd, 0, 3);
}


//----------------------------------------------------------------------
//                WYŁACZANIE TRYBU PROGRAMOWANIA PRZY BRAKU AKTYWNOŚCI
//---------------------------------------------------------------------- 
void wylaczanie_programowania()
{
 //JEŻELI NIE WYKRYTO AKTYWNOŚCI NA ŻADNYM PRZYCISKU
  if ((digitalRead(przycisk_tryb) == HIGH) && (digitalRead(przycisk_plus) == HIGH) && (digitalRead(przycisk_minus) == HIGH))
  {
    licznik = licznik + 1; // LICZNIK = 20 ===> 6 sekund (0.3*20)
    if (licznik == 20)
    {
        przelacz_tryb = false;
        ustaw = 0;
        licznik = 0;
    }
  }
 //JEŻELI  WYKRYTO AKTYWNOŚCI NA JEDNYM Z PRZYCISKÓW
  if ((digitalRead(przycisk_tryb) == LOW) || (digitalRead(przycisk_plus) == LOW) || (digitalRead(przycisk_minus) == LOW))
  {
    licznik = 0;
  }
}


//----------------------------------------------------------------------
//                FUNKCJE PRZYCISKÓW W TRYBIE PROGRAMOWANIA
//----------------------------------------------------------------------
void tryb_programowania() 
{
 //WYKRYWANIE STANU NISKIEGO NA PRZYCISKU TRYBU
  if ((digitalRead(przycisk_tryb) == LOW) && (timer_bounce.available())) 
  {
    ustaw = ustaw + 1; 
    timer_bounce.restart();
  }
 //MENU PROGRAMOWANIA CZASU MODUŁU RTC
  switch (ustaw) {
    case 0 : 
    {
      RRRR = przyciski_plus_minus(RRRR);
      RRRR = zakres_zmiennej(RRRR, 2000, 2500); //ZABEZPIECZENIE PRZED PRZEKROCZENIEM ZAKRESU
      break;
    }
    case 1 : 
    {
      MM = przyciski_plus_minus(MM);
      MM = zakres_zmiennej(MM, 1, 12);  //ZABEZPIECZENIE PRZED PRZEKROCZENIEM ZAKRESU
      break;
    }
    case 2 : 
    {
      DD = przyciski_plus_minus(DD);
      DD = zakres_zmiennej(DD, 1, 31);  //ZABEZPIECZENIE PRZED PRZEKROCZENIEM ZAKRESU
      break;
    }
    case 3 : 
    {
      GODZ = przyciski_plus_minus(GODZ);
      GODZ = zakres_zmiennej(GODZ, 0, 23);  //ZABEZPIECZENIE PRZED PRZEKROCZENIEM ZAKRESU
      break;
    }
    case 4 : 
    {
      MIN = przyciski_plus_minus(MIN);
      MIN = zakres_zmiennej(MIN, 0, 59);  //ZABEZPIECZENIE PRZED PRZEKROCZENIEM ZAKRESU
      break;
    }
    case 5 : 
    {
     //USTAWIENIE NOWEJ DATY 
      zegar.setDateTime(RRRR, MM, DD, GODZ, MIN, /*SEKUNDY*/0);                                                       
     //PRZEJŚCIE DO TRYBU WYŚWIETLANIA
      przelacz_tryb = false;
     //ZEROWANIE MENU PROGRAMOWANIA
      ustaw = 0;
      break;
    }
  }       
}


//----------------------------------------------------------------------
//                UZUPEŁNIANIE DATY ZERAMI DO ZACHOWANIA FORMATOWANIA 
//---------------------------------------------------------------------- 
void uzupelnianie_zer(byte argument)
{
  if (argument < 10)
  {
    lcd.print(0);
  }
}


//----------------------------------------------------------------------
//                MRUGANIE USTAWIANEGO SKŁADNIKA DATY 
//---------------------------------------------------------------------- 
void mruganie_skladnika(byte ustawiany_skladnik, int skladnik)
{
  if ((ustawiany_skladnik  == ustaw) && (digitalRead(przycisk_plus) == HIGH) && (digitalRead(przycisk_minus) == HIGH))
    {
      if (mrugniecie) 
      {
        if (skladnik < 100)
        {
          uzupelnianie_zer(skladnik);
        }
        lcd.print(skladnik);
        mrugniecie = !mrugniecie;
      }
      else 
      {
        if (skladnik > 99)
        {
          lcd.print("    ");
        }
        else
        {
          lcd.print("  ");
        }
        mrugniecie = !mrugniecie;
      }
    }
    else
    {
      if (skladnik < 100)
      {
        uzupelnianie_zer(skladnik);
      }
      lcd.print(skladnik);
    }
}


//----------------------------------------------------------------------
//                WYŚWIETLANIE DATY NA LCD 
//---------------------------------------------------------------------- 
void wyswietlanie_daty()
{
  if (timer_mrugniecie.available())
  {
    lcd.clear();
    lcd.setCursor(3,0);
    mruganie_skladnika(0, RRRR);
    lcd.print("-");
    mruganie_skladnika(1, MM);        
    lcd.print("-");
    mruganie_skladnika(2, DD);
    lcd.setCursor(4,1);
    mruganie_skladnika(3, GODZ);      
    lcd.print(":");
    mruganie_skladnika(4, MIN);    
    lcd.print(":00"); //SEKUNDY
    wylaczanie_programowania();
    timer_mrugniecie.restart(); 
  }
}


//----------------------------------------------------------------------
//                PĘTLA GŁÓWNA PROGRAMU
//----------------------------------------------------------------------
void loop() 
{
  //WYKRYWANIE STANU NISKIEGO NA PRZYCISKU TRYBU
  if (!blokada)
  {
    if ((digitalRead(przycisk_tryb) == LOW) && (timer_bounce.available()))
    {
      przelacz_tryb = !przelacz_tryb;
      timer_bounce.restart();
    }
  }
  //FUNKCJE PRZYCISKU TRYBU PRACY
  if (przelacz_tryb)
  {
    blokada = true; 
    tryb_programowania();
    wyswietlanie_daty();           
  }
  else if (!przelacz_tryb)
  {
    blokada = false;
    tryb_wyswietlania(); 
  }
}





