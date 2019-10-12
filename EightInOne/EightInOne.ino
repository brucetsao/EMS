#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "DHT.h"
#define DHTPIN 8     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT21   // DHT 21 (AM2301)
Adafruit_ADS1115 ads;
//
double sqI[8],sumI[8];
double sampleI[8];
double Irms[8];
uint8_t MacData[6];

IPAddress  Meip ,Megateway ,Mesubnet ;
String MacAddress ;
char ssid[] = "NCNU";      // your network SSID (name)
char pass[] = "";     // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
char server[] = "10.24.21.159";    // name address for Google (using DNS)
//-------------- dht use


DHT dht(DHTPIN, DHTTYPE);

String connectstr ;
String table_temp;
String table1="test1_1";
String table2="test1_2";
String table3="test1_3";
String table4="test1_4";
String table5="test1_5";
String table6="test1_6";
String table7="test1_7";
String table8="test1_8";


WiFiClient client;


double squareRoot(double fg)
{
    double n = fg / 2.0;
    double lstX = 0.0;
    while (n != lstX)
    {
        lstX = n;
        n = (n + fg / n) / 2.0;
    }
    return n;
}
 
void calcIrms(unsigned int Number_of_Samples, float multiplier,double ical)  
{
  int i;
  for(i=0;i<=1;i++)
  {
    for (unsigned int n = 0; n < Number_of_Samples; n++)
    {
        ads.convert_v1(0x48,i+1);
        ads.convert_v1(0x49,i+1);
        ads.convert_v1(0x4A,i+1);
        ads.convert_v1(0x4B,i+1);
        delay(8);
        sampleI[0+i]=ads.read_v1(0x48);
        sampleI[2+i]=ads.read_v1(0x49);
        sampleI[4+i]=ads.read_v1(0x4A);
        sampleI[6+i]=ads.read_v1(0x4B);
        
        sqI[i] = sampleI[i] * sampleI[i];  sumI[i] += sqI[i];
        sqI[2+i] = sampleI[2+i] * sampleI[2+i];  sumI[2+i] += sqI[2+i];
        sqI[4+i] = sampleI[4+i] * sampleI[4+i];  sumI[4+i] += sqI[4+i];
        sqI[6+i] = sampleI[6+i] * sampleI[6+i];  sumI[6+i] += sqI[6+i];
        
      
    }
  }
  for(i=0;i<=7;i++)
  {
    Irms[i] = squareRoot(sumI[i] / Number_of_Samples) * multiplier * ical/100;
    sumI[i] = 0;
  }
//--------------------------------------------------------------------------------------
}



void ShowMac()
{
  
     Serial.print("MAC:");
     Serial.print(MacAddress);
     Serial.print("\n");

}

void setup()   /*----( SETUP: RUNS ONCE )----*/
{

   pinMode(6,OUTPUT);  //Wifi led
   pinMode(5,OUTPUT);  //send data led
   pinMode(7,OUTPUT);  //power on led
   digitalWrite(5,LOW);
   digitalWrite(6,LOW);
   digitalWrite(7,HIGH);
   
   Wire.begin();
    /* ADS1115 @ +/- 4.096V gain (16-bit results) 0.125mV Step*/
    ads.setGain(GAIN_ONE);
    ads.begin();
    Serial.println();
    Serial.println("current meter");
    //MacAddress = GetWifiMac() ; // get MacAddress
    ShowMac() ;       //Show Mac Address
  
    Serial.begin(9600);
    Serial.println("DHTxx test!");
    dht.begin();
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(10000);
  }
  status = WL_IDLE_STATUS;
  Serial.println("Connected to wifi");
  printWifiStatus();

}// END Setup

static int count=0;
void loop()   
{
 
 if(WiFi.status() != WL_CONNECTED)
 {
  digitalWrite(6,LOW);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(10000);
  }
  status = WL_IDLE_STATUS;
  digitalWrite(6,HIGH);
 }
 else 
  digitalWrite(6,HIGH);

 
 connectstr = "" ;

 calcIrms(64, 0.125F,0.00696);  //
 
 for(int i=0; i<=7 ;i++)
 {
  switch (i)
  {
    case 0:
    table_temp=table1;
    break;
    case 1:
    table_temp=table2;
    break;
    case 2:
    table_temp=table3;
    break;
    case 3:
    table_temp=table4;
    break;
    case 4:
    table_temp=table5;
    break;
    case 5:
    table_temp=table6;
    break;
    case 6:
    table_temp=table7;
    break;
    case 7:
    table_temp=table8;
    break;
  }
  
   
   connectstr = "?field0="+table_temp+"&field1=" + String(Irms[i]*110)+"&field2="+String(Irms[i]);
  if (client.connect(server,80)) { // REPLACE WITH YOUR SERVER ADDRESS
    client.print("POST /iot/dht22/dataadd.php");
    client.print(connectstr);
    client.println(" HTTP/1.1");    
//    Serial.print("POST /iot/dht22/dataadd.php");
    Serial.print(connectstr);
//    Serial.println(" HTTP/1.1");     
  //  Serial.println("POST /iot/dht22/dataadd.php HTTP/1.1"); 
    client.print("Host:"); // SERVER ADDRESS HERE TOO
//    Serial.print("Host:"); // SERVER ADDRESS HERE TOO
    client.println(server); // SERVER ADDRESS HERE TOO
//    Serial.println(server); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/x-www-form-urlencoded"); 
//    Serial.println("Content-Type: application/x-www-form-urlencoded"); 
    client.print("Content-Length: "); 
//    Serial.print("Content-Length: "); 
    client.println(connectstr.length()); 
//    Serial.println(connectstr.length()); 
    client.println(); 
    client.print(connectstr); 
//    Serial.print(connectstr); 
  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
    digitalWrite(5,HIGH);
  }

  delay(1000); // WAIT FIVE MINUTES BEFORE SENDING AGAIN
  digitalWrite(5,LOW);
 }

  
  } 


} // END Loop

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

