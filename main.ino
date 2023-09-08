#include <SoftwareSerial.h>
#include <MFRC522.h> //library for rfid sensor
#include <RTClib.h> //library for rtc module
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//define pins for rfid sensor
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
RTC_DS3231 rtc;// Instance of the class for RTC
LiquidCrystal_I2C lcd(0x27, 16, 2);
int state1 = 0;
int state2 = 0;

int greenPin = 6;
int buzzerPin = 5;
int redPin = 7;

String page = "";
char input[12];
int count = 0;
int a;
double total = 0;
int count_prod = 0;

const int numOfCards = 4;//the nuber of cards used. this can change as you want
byte cards[numOfCards][4] = {{0x83, 0x40, 0x23, 0x24},{0x43, 0x9B, 0x1F, 0x24},{0x66, 0xF1, 0x62, 0xAF},{0x53, 0x1E, 0x3A, 0x24}}; // array of UIDs of rfid cards
int n = 0;//n is for the total number of students//j is for to detect the card is valid or not
int numCard[numOfCards]; //this array content the details of cards that already detect or not .
String names[numOfCards] = {"Rakshith R","Sharath M","Shreyas P S Rao","Uday C H"};//student names
String usn[numOfCards] = {"1KS20EC077","1KS20EC093","1KS20EC098","1KS20EC108"};//student sNumbers

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  //setup for display option (serial monitor)
  Serial.println(F("\t\t\t Attendance for the class \n")); // introduction
  Serial.println(F("Date\t\tTime\t\tTeacher Name\tStudent Name\tUSN"));// make four columns  

  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  //setup for RTC module
  while (!Serial); // for Leonardo/Micro/Zero
  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power, lets set the time!"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void rfid() {

  if (Serial.available()) {
    count = 0;
    while (Serial.available() && count < 12) {
      input[count] = Serial.read();
      count++;
      delay(5);
    }
    if (count == 12)
    {
      //Attendance for Teacher 1

      if ((strncmp(input, "4D008CDDDAC6", 12) == 0) && (state1 == 0)) 
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Teacher: A");
        lcd.setCursor(0, 1);
        lcd.print("Class: English");
        digitalWrite(greenPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(1000);
        digitalWrite(greenPin, LOW);
        digitalWrite(buzzerPin, LOW);
        state1 = 1;
        readRFID();
      }   
      else if ((strncmp(input, "4D008CDDDAC6", 12) == 0) && (state1 == 1)) 
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Attendance taken");
        lcd.setCursor(0, 1);
        lcd.print("Student count:");
        lcd.setCursor(14,1);
        lcd.print(n);
        digitalWrite(greenPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(1000);
        digitalWrite(greenPin, LOW);
        digitalWrite(buzzerPin, LOW);
        state1 = 0;
      }

      //Attendance for Teacher 2

      else if ((strncmp(input, "4D00898B4F00", 12) == 0) && (state2 == 0)) 
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Teacher: B");
        lcd.setCursor(0, 1);
        lcd.print("Class: Maths");
        digitalWrite(greenPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(1000);
        digitalWrite(greenPin, LOW);
        digitalWrite(buzzerPin, LOW);
        state2 = 1;
        readRFID();
      } 
      else if ((strncmp(input, "4D00898B4F00", 12) == 0) && (state2 == 1)) 
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Attendance taken");
        lcd.setCursor(0, 1);
        lcd.print("Student count:");
        lcd.setCursor(14,1);
        lcd.print(n);
        digitalWrite(greenPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(1000);
        digitalWrite(greenPin, LOW);
        digitalWrite(buzzerPin, LOW);
        state2 = 0;
      }
      
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid Card!!");
        digitalWrite(redPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(1000);
        digitalWrite(redPin, LOW);
        digitalWrite(buzzerPin, LOW);
      }
      n=0; 
    }
  } 
}


void printLoopLCD()
{//method for display date and time and number of students attendance to the lcd dislpay 
  DateTime now = rtc.now();
  lcd.clear();
  lcd.setCursor (1, 0);
  lcd.print("Welcome to");
  lcd.setCursor (0,1);
  lcd.print("6 Sem ECE");
  delay(1000);
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("Time:");
  lcd.setCursor (5,0);
  myTime();
  lcd.setCursor (0, 1);
  lcd.print("Date:");
  lcd.setCursor (5, 1);
  myDate();
  delay(1000);
}

void readRFID()
{
  if (state1 == 1 || state2 == 1) {

    int j = -1;
    byte card_ID[4];//card UID size 4byte
  
    if ( ! mfrc522.PICC_IsNewCardPresent()) {//look for new card
      return;//got to start of loop if there is no card present
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {// Select one of the cards
      return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
    }

    for (byte i = 0; i < mfrc522.uid.size; i++) {
      card_ID[i] = mfrc522.uid.uidByte[i];
    }
  
    for (int i = 0; i < numOfCards; i++) {
      if (card_ID[0] == cards[i][0] && card_ID[1] == cards[i][1] && card_ID[2] == cards[i][2] && card_ID[3] == cards[i][3]) {
        j = i;
      }
    }
  
    if(j == -1) {//check the card validity
      invalid();
    }

    else if (numCard[j] == 1) { //to check if the card already detect
      alreadyRead(names[j], usn[j], j);
    }

    else {
      //send data to display and save
      logCardData(names[j],usn[j], j);
    }
    delay(1000);
  }

  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Attendance not ");
    lcd.setCursor(5, 1);
    lcd.print("taken");
    delay(1000);
  }

  while((state1 == 1) && (state2 == 1))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Two card read ");
    lcd.setCursor(0, 1);
    lcd.print("Simultaneously");
    digitalWrite(redPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    digitalWrite(buzzerPin, LOW);
    Serial.println("Alert, Two cards read simantaneously!!! ");
  }
  //n=0;
}

void logCardData(String name, String usn, int j)
{
  displayAllow(name,usn);
  DateTime now = rtc.now();
  numCard[j] = 1;//put 1 in the numCard array : numCard[j]={1,1} to let the arduino know if the card was detecting
  n++;

    //display details to the console (serial monitor)
  Serial.print(now.year(),DEC); //print year
  Serial.print(F("-"));
  if(now.month()<10){Serial.print("0");Serial.print(now.month(),DEC);}
  else Serial.print(now.month(),DEC); //print month
  Serial.print(F("-"));
  if(now.day()<10){Serial.print("0");Serial.print(now.day(),DEC);} //if the day is one digit this will display it with zero in front as two digits.
  else Serial.print(now.day(),DEC);
  Serial.print(F("\t")); 
  if(now.hour()<10){Serial.print("0");Serial.print(now.hour(),DEC);}
  else Serial.print(now.hour(),DEC);
  Serial.print(F(":"));
  if(now.minute()<10){Serial.print("0");Serial.print(now.minute(),DEC);}
  else Serial.print(now.minute(),DEC);
  Serial.print(F(":"));
  if(now.second()<10){Serial.print("0");Serial.print(now.second(),DEC);}
  else Serial.print(now.second(),DEC);
  Serial.print(F("\t"));
  if(state1 == 1){Serial.print("A\t\t");}
  else if(state2 == 1){Serial.print("B\t\t");}  
  else Serial.print(F("\t\t"));
  Serial.print(name); //print name of student 
  Serial.print(F("\t"));
  Serial.print(usn); //print usn of student
  Serial.print(F("\t"));
  Serial.println("\n");

}

void alreadyRead(String name, String usn, int j)
{
  DateTime now = rtc.now();
  numCard[j] = 0;
}

void displayAllow(String name, String usn)
{
  lcd.clear();
  lcd.setCursor (6, 0 );
  lcd.print("Welcome!");  
  lcd.setCursor (1, 1 );
  lcd.print(F("Access Granted"));
  digitalWrite(greenPin, HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(greenPin, LOW);
  digitalWrite(buzzerPin, LOW);
  lcd.clear();
  lcd.setCursor (0, 0 );
  lcd.print(name);
  lcd.setCursor (0, 1 );
  lcd.print(usn);
  delay(1000);
}

void myTime()
{
  DateTime now = rtc.now();
  int Secs;
  int Minutes;
  int Hours;
  String myTime;
  Secs = now.second(); 
  Hours = now.hour(); 
  Minutes = now.minute();
  myTime = myTime + Hours + ":" + Minutes + ":" + Secs ;
  lcd.print(myTime);
  myTime = "";
}

void myDate()
{
  DateTime now = rtc.now();
  int Day; 
  int Month;
  int Year;
  String myDate;
  Day = now.day(); 
  Month = now.month(); 
  Year = now.year(); 
  myDate = myDate + " "+ Day + "/" + Month + "/" + Year ;  
  lcd.print(myDate);
  myDate = "";
}

void invalid()
{
  lcd.clear();
  lcd.setCursor (1, 0 );
  lcd.print(F("Invalid Tag!"));
  lcd.setCursor (1, 1 );
  lcd.print(F("Access Denied!"));
  digitalWrite(redPin, HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(redPin, LOW);
  digitalWrite(buzzerPin, LOW);
}

void checkTime()
{
  lcd.clear(); 
  lcd.setCursor (0, 0 );
  lcd.print("Session Ended");  
  lcd.setCursor (3, 1 );
  lcd.print("for the day!!"); 
  delay(59400000);//delay for 16 hours 30 minutes
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();
  if ((now.hour() <= 8 && now.minute() <= 30 ) || now.hour() >= 16) {
    checkTime();
  }
  printLoopLCD();//print details on lcd screen(loop)
  readRFID();
  rfid();
}