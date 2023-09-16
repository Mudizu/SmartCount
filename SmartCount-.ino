// ***************MemoireEEPROM*****************
#include <EEPROM.h>
int adresse = 14;
int flag=0;
int i=0;
char a[2];

//****************ModuleGSM*********************
#include <SoftwareSerial.h>
String IdCompteur = "BT0001";

#define rxPin 10
#define txPin 11
SoftwareSerial gprsSerial(rxPin, txPin);

// *********Courant-Tension***************
#include <EmonLib.h>
EnergyMonitor emon1;
float Irms;
float Vrms;
float apparentPower;
float Energy = 0.00;
unsigned tempsX = 0;
unsigned tempsY = 0;


void setup() {
  gprsSerial.begin(9600);
  Serial.begin(9600);
  // Serial.println(a);

  /************************************* GSM MODULE *************************************/

  Serial.println("Config SIM900...");
  delay(2000);
  Serial.println("Done!...");
  gprsSerial.flush();
  Serial.flush();

  gprsSerial.println("AT");
  delay(1000);
  toSerial();

  gprsSerial.println("AT+CSQ");
  delay(1000);
  toSerial();

  gprsSerial.println("AT+CGATT?");
  delay(1000);
  toSerial();

  gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(2000);
  toSerial();

  // gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"internet\"");
  gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");
  delay(2000);
  toSerial();
  delay(2000);

  gprsSerial.println("AT+SAPBR=1,1");
  delay(2000);
  toSerial();

  gprsSerial.println("AT+SAPBR=2,1");
  delay(2000);
  toSerial();

  // *************CurrentVoltageSensors*******************
  emon1.current(A0, 20);
  emon1.voltage(A1, 230, 1.7);

  // ******************FIN**********************
}

void loop() {
  emon1.calcVI(20, 2000);
  unsigned long time = millis() / 1000;
  Irms = emon1.calcIrms(1480);
  Irms = Irms - 4.9;
  Vrms = emon1.Vrms;
  if (Vrms <= 3) {
    Vrms = 0.00;
  }
  if (Irms <= 0.00) {
    Irms = 0.00;
  }
  //apparentPower = Vrms * Irms;
  apparentPower = 1000.00;


  if (time >= 1) {
    Energy = EEPROM.get(adresse, Energy);
    Energy = Energy + (apparentPower / 3600);
    EEPROM.put(adresse, Energy);
    // Serial.print("time : ");
    // Serial.println(time);
    // Serial.print("Courant : ");
    // Serial.println(Irms);
    // Serial.print("Tension : ");
    // Serial.println(Vrms);
    tempsX++;
  }


  if (tempsX >= 5) {
    Energy = EEPROM.get(adresse, Energy);  //Lire

    // EEPROM.put(adresse, Energy);
    // Serial.println(Energy);
    sendSMS();
    communication();
    // Serial.println(tempsX);
    tempsX = 0;
    // Serial.println(tempsX);
    Energy = 0.00;
    EEPROM.put(adresse, Energy);
    // Serial.println(Energy);
  }
}

void sendSMS() {
  // gprsSerial.print("AT+CMGF=1\r");
  // delay(100);
  // gprsSerial.println("AT + CMGS = \"+243970424054\"");
  // delay(100);

  // // Remplacez par votre propre message
  // gprsSerial.print("L'energie consommee : ");
  // gprsSerial.println(Energy);
  // delay(100);

  // gprsSerial.println((char)26);
  // delay(100);
  // gprsSerial.println();
  // // Donnez un temps au module pour envoyé le message
  // delay(5000);
}
void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}

void communication()
{
  gprsSerial.println("AT+HTTPINIT");
  delay(2000); 
  toSerial();

  String site = "isptlikasi.000webhostapp.com";
  String controller="ComCompteur.php?IdCompteur=BT0001&energie=1000000";
  
  gprsSerial.println("AT+HTTPPARA=\"URL\",\"" + site +"/controllers/"+ controller+"\"");
  delay(5000);
  toSerial();
  
  gprsSerial.println("AT+HTTPACTION=0");
  delay(6000);
  toSerial();
  
  gprsSerial.println("AT+HTTPREAD");
  delay(1000);
  toSerial();
  delay(2000);
  
  gprsSerial.println("");
  gprsSerial.println("AT+HTTPTERM");
  toSerial();
  delay(300);
  
  gprsSerial.println("");  
}