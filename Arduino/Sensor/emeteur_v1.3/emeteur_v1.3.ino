// Module capteur HF Arduino/atmega
//V1.1 : - Ajout mise en veille de l'arduino
//        - Mesure et envoie toute les minutes
// V1.2 : adaptation pour ATMEGA328P-PU
//        - add power command 
//        DHT22 -> digital pin 7
//        RX470 -> digital pin 9 
// V1.3 : - remove useless parts

//
//  DHT 22                : Data on pin 8
//  WL102-341 transmitter : Data on pin 12

 
#include <LowPower.h>
#include <DHT.h>
#include <DHT_U.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile


#define NUM_CAPTEUR 2
#define DHTPIN 8 // pin for DHT22 data
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);//déclaration du capteur
RH_ASK driver;// HF 433Mhz module declaration

char msg[20];
char floatStringH[10];
char floatStringT[10];
int numc = NUM_CAPTEUR;
void setup()
{
    pinMode(7, OUTPUT); // set the digital pin 7 as output
    pinMode(9, OUTPUT); // set the digital pin 9 as output   
    Serial.begin(9600);	  
    if (!driver.init()) Serial.println("init RF failed"); //init module HF
    dht.begin(); //init module DHT
}

void loop()
{ 
   digitalWrite(7, HIGH);       // power on sensor 
   digitalWrite(9, HIGH);       // power on transmitter   
   delay(500);
   // La lecture du capteur prend 250ms
   float h = dht.readHumidity();//on lit l'hygrometrie
   float t = dht.readTemperature();//on lit la temperature en celsius (par defaut)
   
   //On verifie si la lecture a echoue, si oui on quitte la boucle pour recommencer.
   if (isnan(h) || isnan(t))
   {
     //Serial.println("Failed to read from DHT sensor!");
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
   
   bool DStatus = driver.send((uint8_t *)msg, strlen(msg));
//6
   
   driver.waitPacketSent();
   
   digitalWrite(7, LOW); // coupure alimentation capteur
   digitalWrite(9, LOW); // coupure alimentation emmeteur HF     
   // en veille pour 10 min
   for (int i=0; i<600/8; i++) {LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);}
   
}
