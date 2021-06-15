#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>
#include <SD.h>
#include <SPI.h>
#define CLK 2
#define DT 3
#define DHTT1 4
#define DHTT2 5
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define SWITCH1 6
#define SWITCH2 7
#define SWITCH3 8
#define SWITCHR 9
//AL MEASURES ARE IN mm
float counter = 0;
float counter1 = 0;
float counter2 = 0;
float counter3 = 0;
float grams = 0;
float circmed = 0.08639379;
float weight = 0.00025767;
int intgrams = 0;
int intcounter = 0;
int intcounter1 = 0;
int intcounter2 = 0;
int intcounter3 = 0;
int pctusg = 0;
int state = 0;
int currentStateCLK;
int lastStateCLK;
int h = 0;
int t = 0;
int h2 = 0;
int t2 = 0;
int pinoSS = 10;
int switchf1 = LOW;
int switchf2 = LOW;
int switchf3 = LOW;
int switchrs = LOW;
String currentDir = "";
String currentF = "";
String fRead = ""; // file read
boolean inimsg = true;
File myFile1;
File myFile2;
File myFile3;
File myFileSt;
File myFileG1;
File myFileG2;
File myFileG3;
LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTT1, DHTTYPE);
DHT dht2(DHTT2, DHTTYPE);

void setup() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(pinoSS, OUTPUT);
  pinMode(SWITCH1, INPUT);
  pinMode(SWITCH2, INPUT);
  pinMode(SWITCH3, INPUT);
  pinMode(SWITCHR, INPUT);
  lcd.init();
  lcd.backlight();
  dht.begin();
  dht2.begin();
  lastStateCLK = digitalRead(CLK);
  SD.begin(10);
  myFileSt = SD.open("ST.txt");
  while (myFileSt.available()){      
    currentF += myFileSt.readString();            
  }    
  myFileSt.close();
  myFile1 = SD.open("F1.txt");
  while (myFile1.available()){      
    fRead += myFile1.readString();            
  }    
  counter1 = fRead.toFloat();
  intcounter1 = counter1;
  fRead = "";
  myFile1.close();
  myFile2 = SD.open("F2.txt");
  while (myFile2.available()){      
    fRead += myFile2.readString();            
  }
  counter2 = fRead.toFloat();
  intcounter2 = counter2;
  fRead = "";
  myFile2.close();
  myFile3 = SD.open("F3.txt");
  while (myFile3.available()){      
    fRead += myFile3.readString();            
  }
  counter3 = fRead.toFloat();
  intcounter3 = counter3;
  fRead = "";
  myFile3.close(); 
  if (currentF == "F1") counter = counter1;
  if (currentF == "F2") counter = counter2;
  if (currentF == "F3") counter = counter3;  
}

void loop() {
  if (currentF == "F1") state = 1;
  if (currentF == "F2") state = 2;
  if (currentF == "F3") state = 3;  
  if (inimsg) {
    lcd.setCursor(0, 0);
    lcd.print("3DPInfo");    
    lcd.setCursor(0, 3);
    lcd.print("v1.0");
    lcd.setCursor(8, 3);
    lcd.print("by mBaeKwara");   
    inimsg = false;
    delay(2000);
    grams = GramsRet(state);
    intgrams = grams;
    pctusg = (100*intgrams)/1000;
    PrintMenu();
  }
  switchf1 = digitalRead(SWITCH1);
  switchf2 = digitalRead(SWITCH2);
  switchf3 = digitalRead(SWITCH3);
  switchrs = digitalRead(SWITCHR);
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK ) {
    if (digitalRead(DT) != currentStateCLK) {
      counter = counter + circmed;
      intcounter = counter;
      RecCount(state, intcounter);      
      currentDir = "EXT";
      grams = grams + weight;
      intgrams = grams;
      if (currentF == "F1") counter1 = counter;
      if (currentF == "F1") intcounter1 = intcounter;
      if (currentF == "F2") counter2 = counter;
      if (currentF == "F2") intcounter2 = intcounter;
      if (currentF == "F3") counter3 = counter;  
      if (currentF == "F3") intcounter3 = intcounter;
      pctusg = (100*intgrams)/1000;
      PrintMenu();
    } else {
      counter = counter - circmed;      
      intcounter = counter;
      RecCount(state, intcounter);      
      currentDir = "RET";
      grams = grams - weight;
      intgrams = grams;
      if (currentF == "F1") counter1 = counter;
      if (currentF == "F1") intcounter1 = intcounter;
      if (currentF == "F2") counter2 = counter;
      if (currentF == "F2") intcounter2 = intcounter;
      if (currentF == "F3") counter3 = counter;
      if (currentF == "F3") intcounter3 = intcounter;
      pctusg = (100*intgrams)/1000;
      PrintMenu();
    }
  }
  if (switchf1 == HIGH){
    currentF = "F1";
    counter = counter1;
    ChngFila(currentF);
    grams = GramsRet(state);
    intgrams = grams;
    pctusg = (100*intgrams)/1000;
    PrintMenu();    
  }else if (switchf2 == HIGH){
    currentF = "F2";
    counter = counter2;
    ChngFila(currentF);
    grams = GramsRet(state);
    intgrams = grams;
    pctusg = (100*intgrams)/1000;
    PrintMenu();
  }else if (switchf3 == HIGH){
    currentF = "F3";
    counter = counter3;
    ChngFila(currentF);
    grams = GramsRet(state);
    intgrams = grams;
    pctusg = (100*intgrams)/1000;
    PrintMenu();
  }else if (switchrs == HIGH){
    if (currentF == "F1") {
     ResetFila(1);
    }else if (currentF == "F2"){
      ResetFila(2);
    }else{
      ResetFila(3);
    }
    PrintMenu();
  }
  lastStateCLK = currentStateCLK;
}
//teste
void RecCount(int Ftype, int count){
  if (Ftype == 1) {
    myFile1 = SD.open("F1.txt", O_WRITE);
    myFile1.seek(0);
    myFile1.print(count,DEC);
    myFile1.print("         ");
    myFile1.close();
  }else if (Ftype == 2){
    myFile2 = SD.open("F2.txt", O_WRITE);
    myFile2.seek(0);
    myFile2.print(count,DEC);
    myFile2.print("         ");
    myFile2.close();
  }else if (Ftype == 3){
    myFile3 = SD.open("F3.txt", O_WRITE);
    myFile3.seek(0);
    myFile3.print(count,DEC);
    myFile3.print("         ");
    myFile3.close();
  }
}

void PrintMenu(){
  h = dht.readHumidity(); 
  t = dht.readTemperature();
  h2 = dht2.readHumidity(); 
  t2 = dht2.readTemperature();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("F1:");
  lcd.print(intcounter1);
  lcd.setCursor(10, 0);
  lcd.print("F2:");
  lcd.print(intcounter2);
  lcd.setCursor(0, 1);
  lcd.print("F3:");
  lcd.print(intcounter3);
  lcd.setCursor(10, 1);
  lcd.print("Gs:");
  lcd.print(intgrams);
  lcd.setCursor(0, 2);
  lcd.print("T1:");
  lcd.print(t);
  lcd.print("c");
  lcd.print(" T2:");
  lcd.print(t2);
  lcd.print("c");
  lcd.setCursor(14, 2);
  lcd.print("P:");  
  lcd.print(pctusg);
  lcd.print("%");
  lcd.setCursor(0, 3);
  lcd.print("H1:");
  lcd.print(h);
  lcd.print("%");
  lcd.print(" H2:");
  lcd.print(h2);
  lcd.print("%");
  lcd.setCursor(14, 3);
  lcd.print(currentF);
  lcd.print(" ");
  lcd.print(currentDir);
}

void ChngFila(String cFila){
    myFileSt = SD.open("ST.txt", O_WRITE);
    myFileSt.seek(0);
    myFileSt.print(cFila);
    myFileSt.close();
}

void ResetFila(int xType){
  if (xType == 1) {
    myFile1 = SD.open("F1.txt", O_WRITE);
    myFile1.seek(0);
    myFile1.print(300,DEC);
    myFile1.print("         ");
    myFile1.close();
    counter1 = 300;    
    intcounter1 = counter1;
    GramsChng(xType);
  }else if (xType == 2){
    myFile2 = SD.open("F2.txt", O_WRITE);
    myFile2.seek(0);
    myFile2.print(300,DEC);
    myFile2.print("         ");
    myFile2.close();
    counter2 = 300;
    intcounter2 = counter2;
    GramsChng(xType);
  }else {
    myFile3 = SD.open("F3.txt", O_WRITE);
    myFile3.seek(0);
    myFile3.print(300,DEC);
    myFile3.print("         ");
    myFile3.close();
    counter3 = 300;
    intcounter3 = counter3;
    GramsChng(xType);
  }
  counter = 300;
  intcounter = counter;
  grams = 0.8947743;
  intgrams = grams;
  pctusg = (100*intgrams)/1000;
}

float GramsRet(int zType){
  float retg;
  if (zType == 1){
    myFileG1 = SD.open("G1.txt");
    while (myFileG1.available()){      
      fRead += myFileG1.readString();            
    }    
    retg = fRead.toFloat();    
    myFileG1.close();
  }else if (zType == 1){
    myFileG2 = SD.open("G2.txt");
    while (myFileG2.available()){      
      fRead += myFileG2.readString();            
    }    
    retg = fRead.toFloat();    
    myFileG2.close();
  }else{
    myFileG3 = SD.open("G3.txt");
    while (myFileG3.available()){      
      fRead += myFileG3.readString();            
    }    
    retg = fRead.toFloat();    
    myFileG3.close();
  }  
  return retg;
}

void GramsChng(int gType){
  if (gType == 1){
    myFileG1 = SD.open("G1.txt", O_WRITE);
    myFileG1.seek(0);
    myFileG1.print(0.8947743,DEC);
    myFileG1.print("         ");
    myFileG1.close();
  }else if (gType == 2){
    myFileG2 = SD.open("G2.txt", O_WRITE);
    myFileG2.seek(0);
    myFileG2.print(0.8947743,DEC);
    myFileG2.print("         ");
    myFileG2.close();
  }else{
    myFileG3 = SD.open("G3.txt", O_WRITE);
    myFileG3.seek(0);
    myFileG3.print(0.8947743,DEC);
    myFileG3.print("         ");
    myFileG3.close();
  }
}
