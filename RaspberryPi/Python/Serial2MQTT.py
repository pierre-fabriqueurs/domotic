# read a jason string from serial port and publish via MQTT
# pierre@fabriqueurs.com
#
#uses the Python MQTT client from the Mosquitto project
#http://mosquitto.org
#
#inspired from readserial.py (Andy Piper http://andypiper.co.uk)


import serial
import paho.mqtt.client as paho
import os
import time
import json

serialdev = '/dev/ttyUSB0'
broker = "127.0.0.1"
port = 1883


#called on exit
#close serial, disconnect MQTT
def cleanup():
    print "Ending and cleaning up"
    ser.close()
    mqttc.disconnect()

    
while True:
   try:  
       time.sleep(5)
       print "Connecting... ", serialdev
       #connect to serial port
       ser = serial.Serial(serialdev, 9600)
       print "Connected to serial port"
       break
   except:
       print "Failed to connect serial"


try:
    ser.flushInput()
    #create an mqtt client
    mypid = os.getpid()
    client_uniq = "arduino_pub_"+str(mypid)
    mqttc = paho.Client(client_uniq)

    #read data from serial. If jason format, connect, publish and disconnect
    while True:
        line = ser.readline()
        print(line)
        try:              
            data = json.loads(line)
        except:
            print "not a jason format"
            continue
        while True:
            try:
                mqttc.connect(broker, port, 60)
                print "Connected to broker"
                break
            except:
                time.sleep(5)
                print "Failed to connect broker"
                
        topic = "arduino/sensor"+data["capt_info"]["id"]
        print "topic: "+topic
        mqttc.publish(topic, line)
        print "Published"
        time.sleep(5)
        mqttc.disconnect()
        print "Diconnected from broker"

# handle app closure
except (KeyboardInterrupt):
    print "Interrupt received"
    cleanup()
except (RuntimeError):
    print "uh-oh! time to die"
    cleanup()
