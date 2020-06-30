#include <SoftwareSerial.h> // for esp8266 interface to arduino to serial communication
int srdata;//store sensor data from sensor module
int prdata;//store mapped data of the sensor data
int pump_status;
SoftwareSerial esp8266(3,4);
//put your ssid and password details
#define SSID "NTE"
#define PASS "nepalnte"

//now define send AT function to send at command
//used multiple time to send at command to esp8266 wifi module
String sendAT(String command, const int timeout)
{
  String response = "";
  esp8266.print(command);//print serial comd
  long int time = millis();
  //based on AT command esp8266 response and stored on a character 
  while((time + timeout) > millis())
  {
    while(esp8266.available())
    {
      
        char c = esp8266.read();//read on a character variable
        response += c;//character is added to variable
      }
    }
    Serial.print(response);//function finally print response and give output
    return response;
  }
//initialize once and process of system
//connect esp8266 with wifi
void setup() {
  
 Serial.begin(9600);
 esp8266.begin(9600);//initiate serial comunication with esp8266
 sendAT("AT+RST\r\n",2000);//calling sendAT command 
  sendAT("AT\r\n",1000);
   sendAT("AT+CWMODE=1\r\n",1000);//setting esp in station mode
    sendAT("AT+CWJAP=\""SSID"\",\""PASS"\"\r\n",2000);//connection to wifi network
    while(!esp8266.find("OK"))//checking wifi connctivity
    {
      
      }
      sendAT("AT+CIFSR\r\n",1000);//once connected DISPLAY WIFI DETAILS IP AND MAC
       sendAT("AT+CIPMUX=0\r\n",1000); //SET SINGLE CONNECTION
 //Serial.println("Reading From the Sensor ...");
 
pinMode(A0,INPUT);//assigning input and output of arduino to read analog data
pinMode(8,OUTPUT);//control pump on/off


}
//read data from soil moisture and operate pump automatically and then upload data
void loop()
{
  
// put your main code here, to run repeatedly:
srdata=analogRead(A0);
prdata=map(srdata,0,1023,100,0);
Serial.print("Sensor Data  :");
Serial.println(prdata);
String sensor_value = String(prdata);//convert analog signal into string variable

if(prdata<50) //controling the pump based on sensor data)
 { 
  digitalWrite(8,HIGH);
  pump_status = 100;
 }
 else
 {
  digitalWrite(8,LOW);
  pump_status = 0;
  }
  String pump = String(pump_status);
  updateTS(sensor_value,pump);
  delay(2000);
  }
 
//send data to thingspeak server through updateTS()
void updateTS(String T,String P)
  {
    
    Serial.print("");
    sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\80\r\n",1000);//connection establish
        delay(2000);
    String cmdlen;
    String cmd = "GET /update?key=Your_Key_Here&field1="+T+"&field2="+P+"\r\n";//fields is ur field in ur  channel on thingspeak server
    cmdlen = cmd.length();//used to length of cmd
     sendAT("AT+CIPSEND="+cmdlen+"\r\n",2000);//send data note length should be less than 2024 bytes to avoid data loss
     esp8266.print(cmd);
     Serial.println("");
      sendAT("AT+CIPCLOSE\r\n",2000);//CLOSE TCP CONNECTION
      Serial.println("");
      delay(15000);//delay for 15 sec 

      
    
    
    }
