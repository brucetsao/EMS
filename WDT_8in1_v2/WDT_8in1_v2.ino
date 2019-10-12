#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "wiring_watchdog.h"

Adafruit_ADS1115 ads;
//
double sqI[8],sumI[8];
double sampleI[8];
double Irms[8];
uint8_t MacData[6];
int voltage[8]={110,110,110,110,
                110,110,110,110};
IPAddress  Meip ,Megateway ,Mesubnet ;
String MacAddress="" ;
char ssid[] = "NCNU";      // your network SSID (name)
char pass[] = "";     // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
char server[] = "163.22.24.51";    // name address for Google (using DNS)




String connectstr ;
String table_temp;
String table1="Sf0038ce7a8e41";  // 資料表名稱(MAC組成)
String table2="Sf0038ce7a8e42";
String table3="Sf0038ce7a8e43";
String table4="Sf0038ce7a8e44";
String table5="Sf0038ce7a8e45";
String table6="Sf0038ce7a8e46";
String table7="Sf0038ce7a8e47";
String table8="Sf0038ce7a8e48";

WiFiClient client;

double squareRoot(double fg)  //開根號
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
  for (unsigned int n = 0; n < Number_of_Samples; n++)    
  {
  for(i=0;i<=1;i++)  
  { 
        ads.convert_v1(0x48,i+1);
        ads.convert_v1(0x49,i+1);
        ads.convert_v1(0x4A,i+1);
        ads.convert_v1(0x4B,i+1);
        sampleI[0+i]=ads.read_v1(0x48);
        sampleI[2+i]=ads.read_v1(0x49);
        sampleI[4+i]=ads.read_v1(0x4A);
        sampleI[6+i]=ads.read_v1(0x4B)     
        sqI[i] = sampleI[i] * sampleI[i];  sumI[i] += sqI[i];
        sqI[2+i] = sampleI[2+i] * sampleI[2+i];  sumI[2+i] += sqI[2+i];
        sqI[4+i] = sampleI[4+i] * sampleI[4+i];  sumI[4+i] += sqI[4+i];
        sqI[6+i] = sampleI[6+i] * sampleI[6+i];  sumI[6+i] += sqI[6+i]; 
       wdt_reset()      
    }
  }
  for(i=0;i<=7;i++)  {
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
   wdt_enable(8000);

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
    MacAddress = GetWifiMac() ; // get MacAddress
    ShowMac() ;       //Show Mac Address 
    Serial.begin(9600);
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
    wdt_reset();
    // wait 10 seconds for connection:
    delay(5000);
  }
  status = WL_IDLE_STATUS;
  Serial.println("Connected to wifi");
  printWifiStatus();

}// END Setup
static int count=0;
void loop()   
{
  wdt_reset();
 
 if(WiFi.status() != WL_CONNECTED)
 {
    digitalWrite(6,LOW);
    while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid);
    wdt_reset();
    // wait 10 seconds for connection:
    delay(5000);
  }
  status = WL_IDLE_STATUS;
  digitalWrite(6,HIGH);
 }
 else 
  digitalWrite(6,HIGH);

 
 connectstr = "" ;

 calcIrms(3840, 0.125F,9.6);  //
 
 for(int i=0; i<=7 ;i++)
 {
  wdt_reset();
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

  wdt_disable();
   
   connectstr = "?tablename="+table_temp+"&voltage=" + String(voltage[i])+"&current="+String(Irms[i])+"&power="+String(Irms[i]*voltage[i])+"&mac="+MacAddress;
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
    delay(30);
  }
  wdt_enable(8000);
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

String GetWifiMac()
{
   String tt ;
   String t1,t2,t3,t4,t5,t6 ;
   WiFi.status();    //this method must be used for get MAC
   WiFi.macAddress(MacData);
  
   Serial.print("Mac:");
   Serial.print(MacData[0],HEX) ;
   Serial.print("/");
   Serial.print(MacData[1],HEX) ;
   Serial.print("/");
   Serial.print(MacData[2],HEX) ;
   Serial.print("/");
   Serial.print(MacData[3],HEX) ;
   Serial.print("/");
   Serial.print(MacData[4],HEX) ;
   Serial.print("/");
   Serial.print(MacData[5],HEX) ;
   Serial.print("~");
   
   t1 = print2HEX((int)MacData[0]);
   t2 = print2HEX((int)MacData[1]);
   t3 = print2HEX((int)MacData[2]);
   t4 = print2HEX((int)MacData[3]);
   t5 = print2HEX((int)MacData[4]);
   t6 = print2HEX((int)MacData[5]);
  tt = (t1+t2+t3+t4+t5+t6) ;
Serial.print(tt);
Serial.print("\n");
  
  return tt ;
}

String  print2HEX(int number) {
  String ttt ;
  if (number >= 0 && number < 16)
  {
    ttt = String("0") + String(number,HEX);
  }
  else
  {
      ttt = String(number,HEX);
  }
  return ttt ;
}


