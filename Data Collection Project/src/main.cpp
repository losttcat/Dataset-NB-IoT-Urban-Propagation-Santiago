#include <Arduino.h>
#include <StarterKitNB.h>
#include <functions.cpp>

#include <SparkFun_SHTC3.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

//Definiciones

StarterKitNB sk;
SHTC3 mySHTC3;

//General def
int i = 0;
int j = 0;
bool DirCreated = false;

//GPS
bool GPSretry = false; 
String GPSmsg = "";
String lat_str = "";
String lon_str = "";

//KPIs
int indexVariables;
int indexVariablesNext;
String resp="";
String CI="";
String CE="";
String RSSI="";
String RSRP="";
String SINR="";
String RSRQ="";
String BER="";
String TAC="";
String msg="";
String CQI="";
String nums="-0123456789";

//NB connect
String apn = "m2m.entel.cl";
String user = "entelpcs";
String band = "B28 LTE";
String pw = "entelpcs";
String network = "NB";

String resp1;
String resp2;

SSD1306Wire display(0x3c, SDA, SCL);  

// temp&hum
String hum = "";
String temp = "";

// Pantalla OLED
void printBuffer(String resp1, String resp2) {
  display.setLogBuffer(5, 30);

  int indexVariables;
  int indexVariablesNext;
  String resp="";
  String CI="";
  String RSRP="";
  unsigned int len1;
  unsigned int len2;

  indexVariables = resp1.indexOf(",");
  indexVariables = resp1.indexOf(",", indexVariables+1);
  indexVariablesNext = resp1.indexOf(",", indexVariables+1);
  indexVariables = indexVariablesNext;
  indexVariablesNext = resp1.indexOf(",", indexVariables+1);
  CI = resp1.substring(indexVariables+1, indexVariablesNext);        // Cell ID
  
  indexVariables = resp2.indexOf(","); 
  indexVariablesNext = resp2.indexOf(",", indexVariables+1);
  indexVariables = indexVariablesNext;
  indexVariablesNext = resp2.indexOf(",", indexVariables+1);
  RSRP = resp2.substring(indexVariables+1, indexVariablesNext);      // RSRP dBm

  String charCI = "Cell ID: " + CI;
  String charRSRP = "iter: " + RSRP;
  // Some test data
  const char* test[] = {
    "hello",
    "World" ,
    "----",
  };

  len1 = charCI.length()+1;
  char buf1[len1];
  charCI.toCharArray(buf1,len1);
  test[0] = buf1;

  len2 = charRSRP.length()+1;
  char buf2[len2];
  charRSRP.toCharArray(buf2,len2);
  test[1] = buf2;
  

  for (uint8_t i = 0; i < 3; i++) {
    display.clear();
    // Print to the screen
    display.println(test[i]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
    delay(500);
  }
}

// Temp&Hum
void errorDecoder(SHTC3_Status_TypeDef message)             
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.println("Nominal"); break;
    case SHTC3_Status_Error : Serial.println("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.println("CRC Fail"); break;
    default : Serial.println("Unknown return code"); break;
  }
}

// empieza el código
void setup() {
  
  display.init();

  display.setContrast(255);

  //SDcard bien montada
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
  Wire.begin();
  errorDecoder(mySHTC3.begin()); 
  sk.Setup();
  delay(500);
  sk.UserAPN(apn, user, pw);
  delay(500);
  sk.Connect(apn, band, network);
  delay(500);
  sk.StopPSM();
  delay(500);
  
}

void loop() { 

  // Este if es para tomar la traza GPS
  if (i<=9)
  { 

    //Toma traza GPS
    if (GPSretry == false){
        sk.StartGPS(); 
    }

    delay(2000);
    GPSmsg = sk.PositionGPS();
    Serial.println(GPSmsg);

    indexVariables = GPSmsg.indexOf(",");
    indexVariablesNext = GPSmsg.indexOf(",", indexVariables+1);

    lat_str = GPSmsg.substring(indexVariables+1, indexVariablesNext);        // Latitude

    indexVariables = indexVariablesNext;
    indexVariablesNext = GPSmsg.indexOf(",", indexVariables+1);
    lon_str = GPSmsg.substring(indexVariables+1, indexVariablesNext);      // Longitude

    // Si no logra tomar la traza GPS, lo tiene que volver a intentar
    if (GPSmsg == "Null"){
      GPSretry = true;
      delay(500); 
    }

    // Si lo logra tomar, para el GPS
    else {
      GPSretry = true;
      //sumLat = sumLat + lat_str.toDouble();
      Serial.println(lat_str);
      //Serial.println(sumLat);
      //sumLon = sumLon + lon_str.toDouble();
      Serial.println(lon_str);
      // Serial.println(sumLon);
      Serial.println("...");

      String lat_str = "";
      String lon_str = "";
      i++;
    }
    resp1 = "GPS";
    resp2 = String(i);
    printBuffer(resp1, resp2);
    delay(500);
    display.clear();
    delay(500);
  }

  // Este else es para tomar los KPIs 
  else if(j<=100)
  {
    // crea el directorio en la SD card
    String Dir = "/"+ lat_str+"_"+lon_str+".csv";
    char DirBuff[Dir.length()+1];
    Dir.toCharArray(DirBuff, Dir.length()+1);

    if (!DirCreated)
    {
      sk.EndGPS();
      delay(2000);
      DirCreated = true;
      deleteFile(SD, DirBuff);

      writeFile(SD, DirBuff, "Cell ID, TAC, RSSI, RSRP, SINR, RSRQ, CE, BER, Temp, Hum\n");
      delay(500);
    }
    
    // Reconecta a NB
    if (!sk.ConnectionStatus()) 
    {
      sk.Reconnect(apn, band);  
      delay(500);
    }

    // Toma los KPIs
    resp = sk.bg77_at((char *)"AT+QCSQ", 500, false);
    indexVariables = resp.indexOf(","); 
    indexVariablesNext = resp.indexOf(",", indexVariables+1);
    RSSI = resp.substring(indexVariables+1, indexVariablesNext);      // RSSI dBm
    indexVariables = indexVariablesNext;
    indexVariablesNext = resp.indexOf(",", indexVariables+1);
    RSRP = resp.substring(indexVariables+1, indexVariablesNext);      // RSRP dBm
    indexVariables = indexVariablesNext;
    indexVariablesNext = resp.indexOf(",", indexVariables+1);
    SINR = resp.substring(indexVariables+1, indexVariablesNext);      // SINR dB
    indexVariables = indexVariablesNext;
    indexVariablesNext++;
    while (nums.indexOf(resp[indexVariablesNext])>= 0)
    {
      indexVariablesNext++;
    }
    RSRQ = resp.substring(indexVariables+1, indexVariablesNext);      // RSRQ dB
    delay(100);

    resp = sk.bg77_at((char *)"AT+CEREG?", 500, false);
    indexVariables = resp.indexOf(",");
    indexVariables = resp.indexOf(",", indexVariables+1);
    indexVariablesNext = resp.indexOf(",", indexVariables+1);
    TAC = resp.substring(indexVariables+1, indexVariablesNext);       // Tracker Area Code
    indexVariables = indexVariablesNext;
    indexVariablesNext = resp.indexOf(",", indexVariables+1);
    CI = resp.substring(indexVariables+1, indexVariablesNext);        // Cell ID
    delay(100);

    resp = sk.bg77_at((char *)"AT+QCFG=\"celevel\"", 500, false);
    indexVariables = resp.indexOf(","); 
    indexVariablesNext = indexVariables+2;
    CE = resp.substring(indexVariables+1, indexVariablesNext);        // CE lvl
    delay(100);

    resp = sk.bg77_at((char *)"AT+CSQ", 500, false);
    indexVariables = resp.indexOf(","); 
    indexVariablesNext = indexVariables+1; 
    while (nums.indexOf(resp[indexVariablesNext])>= 0)
    {
      indexVariablesNext++;
    }
    BER = resp.substring(indexVariables+1, indexVariablesNext);       // BER
    delay(100);

    SHTC3_Status_TypeDef result = mySHTC3.update();
    hum = String(mySHTC3.toPercent());                                // humidity %
    temp = String(mySHTC3.toDegC());                                  // temperature C°
    delay(100);

    // Los guarda en la SD card
    msg = ""+ CI+","+TAC+","+ RSSI+","+ RSRP+","+ SINR+","+RSRQ+","+CE+","+BER+","+temp+","+hum+"\n";
    char msgBuff[msg.length()+1];
    msg.toCharArray(msgBuff, msg.length()+1);
    Serial.println(j);
    appendFile(SD, DirBuff, msgBuff);
    Serial.println(CI);
    Serial.println(RSSI);
    j++;
    resp1 = CI;
    resp2 = String(j);
    printBuffer(resp1, resp2);
    delay(500);
    display.clear();
    delay(500);
  }



}

