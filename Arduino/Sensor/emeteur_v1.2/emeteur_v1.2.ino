// V1.1 : - Ajout mise en veille de l'arduino
//        - Mesure et envoie toute les minutes
// V1.2 : adaptation pour ATMEGA328P-PU
//        - initial delay from 2000 to 4000
//        - add Led indicators (red -> digital pin 9, green -> digital pin 10)

#include <LowPower.h>
#include <DHT.h>
#include <DHT_U.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile


#define NUM_CAPTEUR 1
#define DHTPIN 8 // broche ou l'on a branche le capteur DHT22
#define DHTTYPE DHT22 // DHT 22 (AM2302)

// Emmetor Data pin connected to digital pin 12 
DHT dht(DHTPIN, DHTTYPE);//déclaration du capteur
RH_ASK driver;// declaration du module HF 433Mhz

char msg[20];
char floatStringH[10];
char floatStringT[10];
int numc = NUM_CAPTEUR;
void setup()
{
    pinMode(9, OUTPUT);          // sets the digital pin 9 as output - nok status red led
    pinMode(10, OUTPUT);          // sets the digital pin 9 as output - ok status green led
    Serial.begin(9600);	  
    if (!driver.init()) Serial.println("init RF failed"); //init module HF
    dht.begin(); //init module DHT
}

void loop()
{ 
   digitalWrite(10, HIGH);       // sets the digital pin 10 on
   delay(4000);
   // La lecture du capteur prend 250ms
   // Les valeurs lues peuvet etre vieilles de jusqu'a 2 secondes (le capteur est lent)
   float h = dht.readHumidity();//on lit l'hygrometrie
   float t = dht.readTemperature();//on lit la temperature en celsius (par defaut)
   
   //On verifie si la lecture a echoue, si oui on quitte la boucle pour recommencer.
   if (isnan(h) || isnan(t))
   {
     //Serial.println("Failed to read from DHT sensor!");
     for (int i=0; i<2; i++)
     {
        digitalWrite(9, HIGH);       // sets the digital pin 9 on
        delay(1000);                  // waits for a second
        digitalWrite(9, LOW);        // sets the digital pin 9 off
        delay(1000);
     }         
     return;
   }
   
   // Calcul de l'indice de temperature en Celsius
   float hic = dht.computeHeatIndex(t, h, false);
   
   //Affichages :
   Serial.print("Humidite: ");
   Serial.print(h);
   Serial.print(" \%\t");
   Serial.print("Temperature: ");
   Serial.print(t);
   Serial.print(" *C \t");
 
   Serial.print("Indice de temperature: ");
   Serial.print(hic);
   Serial.println(" *C ");
    
   dtostrf(h,4,2,floatStringH); // car implementation sprintf arduino ne gere pas les float
   dtostrf(t,4,2,floatStringT); // car implementation sprintf arduino ne gere pas les float 
   sprintf(msg, "S:%d - H:%s - T:%s", numc,floatStringH,floatStringT);
   
   driver.send((uint8_t *)msg, strlen(msg));
   driver.waitPacketSent();

   //for (int i=0; i<60/8; i++) {LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);}
   digitalWrite(10, LOW);       // sets the digital pin 10 of
   LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
}
