#include <dht.h>
#include <Adafruit_Sensor.h>
#include <SD.h> //load sd card library
#include <SPI.h> //load spi communication library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHT11_PIN 6

dht DHT;
float currentTemp;
float fTemp;
int doorSensorPin = A1; //declare temperature sensor pin analog1
int sensorPin = A0; //declare sensor pin on analog 5
int statusOkPin = 8; // green led pin
int statusNotOkPin = 7; // red led pin
int sensorPinReading = analogRead(sensorPin);
int doorSensorReading = analogRead(doorSensorPin);
int screenDelay = 1000; //declare a universal print var to 1 sec
int smallDelay = 1000; //declare a delay so the user can read the info displayed by the screen
int chipSelect = 4; //chipSelect pin
File pSensorData; //variable for working with file object

LiquidCrystal_I2C lcd(0x27,20,4); // set lcd address

void setup() {
  Serial.begin(9600);  // initialize serial console at 9600 baud rate
  pinMode(doorSensorPin,INPUT); // temperature pin is declared as an input
  pinMode(sensorPin,INPUT); // sensor pin is declared as input
  pinMode(statusOkPin,OUTPUT); // OK status led
  pinMode(statusNotOkPin,OUTPUT); // NOT OK status led
  pinMode(10, OUTPUT); // <<read about this>>  sd card library needs pin 10 setup as output. otherwise things wont be happy
  SD.begin(chipSelect); //initialize sd card with chipSelect connected to pin 4
  lcd.init(); //initialize the lcd screen
  lcd.backlight(); // for whatever reason, you need to call this function to illuminate the screen. it's useless without the backlight. why do i have to call the function??? why?
};

void loop() {
  Serial.println(doorSensorReading);
  delay(1000);
  lcd.clear();
  pSensorData = SD.open("PData.txt", FILE_WRITE); //open this file is on the sd card open it. if not it will create it, it will keep adding data onto it even if it has previous data on it
  delay(smallDelay);

  //  currentTemp = analogRead(doorSensor);
  //  currentTemp = currentTemp * (5000/1024);
  //  currentTemp = currentTemp - 500 / 10;
  //  delay(screenDelay);
  //  lcd.print(currentTemp);
  //  delay(screenDelay);
  //  lcd.clear();

  if (pSensorData.available() >= 5){  // CHECKS IF CARD IS INSERTED DURING PROGRAM OPERATION.if this returns a value less than 5 the card is not inserted
     check(); // runs setup again to check for the card else it will continue on with the program/ logging process
   };

  if (SD.exists("PData.txt")==true){log(); //CHECKS FOR FILE IN CARD. might be obsolete
    }else{
      check(); // runs setup again to check for the card else it will continue on with the program/ logging process
  };
};

void check() { //this loop's primary function is to re-read the chipSelect pin to see if the SD card is inserted. 
  digitalWrite(statusNotOkPin,HIGH); //if you're in this loop the sd card is not detected. it will illuminate a red led. red is bad. only when the sd is inserted it will turn off in log() below.
  lcd.print("SD card Missing  "); // sd card status
  delay(1000);
  lcd.clear();
  SD.begin(chipSelect); //re-read the chipSelect pin. if it returns 0 a card is inserted. if it returns a number > 5 it's not inserted
};

void log() {  // if the file search returns true execute exists message for only one second and then continue on with your day  
  digitalWrite(statusNotOkPin,LOW); //writes low to status not okay pin, if it's been triggered
  digitalWrite(statusOkPin,HIGH); // writes high to the green led. if you're in this loop you're golden :D
  DHT.read11(DHT11_PIN); // read values from the dht pin variable, digital pin 6 in this case
  fTemp = DHT.temperature * 1.8 + 32;
  lcd.setCursor(0,0);         // set cursor to first row
  lcd.print("Temp: ");        // print template and values stored by the variables
  lcd.print(fTemp);           //variable for converted temperature
  lcd.print((char)223);       // special degree character
  lcd.print("F");             //
  lcd.setCursor(0,1);         // set cursor to second row
  lcd.print("Humidity: ");    //
  lcd.print(DHT.humidity);    // variable for humidity
  lcd.print("%");             //
  delay(1000);         //
  lcd.clear();                // 
  pSensorData.print(sensorPinReading); //write tempF to the sd card
  pSensorData.println(","); //write comma to the line
  pSensorData.close(); //close the file 
  lcd.print("Logged:");
  lcd.print(analogRead(sensorPinReading));
  delay(1000);
  lcd.clear();
  digitalWrite(statusOkPin,LOW);
};
