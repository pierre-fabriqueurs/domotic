// recepteur v2.1
// Test pour test liaison serie avec Raspi Home Assistant (format JSON)
//
// Use RadioHead to receive messages
// (one-way) receiver with an Rx-B1 module

#include <ArduinoJson.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver; // rx on pin 11
//RH_ASK driver(speed=2000, rxPin=11, txPin=12, pttPin=10, pttInverted=false); 
StaticJsonBuffer<200> jsonBuffer;
     
void setup()
{
    Serial.begin(9600);	
    if (!driver.init())
         Serial.println("init failed");
    else
        Serial.println("init ok");
}

void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
    
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        String str = (char*)buf;
        //Serial.println(str);
        
        JsonObject& root = jsonBuffer.createObject();    
        // extract values from str to fill the json structure (regex method would be more clean...)             
        //root["sensor"] = "S" + str.substring(2,3);
        JsonObject& capt_info = root.createNestedObject("capt_info");
        JsonObject& values = root.createNestedObject("values");
        capt_info["Type"] = "DHT22";
        capt_info["id"] = "S"+ str.substring(2,3);
        //values["Temp"] = str.substring(18,23).toFloat();
        values["Temp"] = str.substring(18,22);
        //values["Hydro"] = str.substring(8,13).toFloat();
        values["Hydro"] = str.substring(8,13);     
        
        //root.printTo(Serial);
        //root.prettyPrintTo(Serial);
        root.printTo(Serial);
        jsonBuffer.clear();
        Serial.println();

    }
}
