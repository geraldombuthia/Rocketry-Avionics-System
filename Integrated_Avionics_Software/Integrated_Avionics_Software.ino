/*
   This is the actual avionics system driver code
   Designed to collect data from the MPU6050, BMP280,
   and write them into the SD card reader Module.
   Outputs include LEDs and a Buzzer and inputs include
   a button.


   Written and designed by
   Gerald Mbuthia
   Electronics and Embedded Systems Engineer
   Omarichet.Space
*/
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_BMP280 bmp; // For I2C
Adafruit_MPU6050 mpu;


uint8_t red = 26;
uint8_t yellow = 25;
uint8_t blue = 32;
uint8_t amber = 33; // Green
uint8_t btn = 12;
int id = 0;
const int BLINK_INTERVAL = 50;
const int BLINK_INTERVAL2 = 300;
const int debounceDelay = 50;
unsigned long previousMillis = 0;
unsigned long prevTime = 0;
unsigned long lastDebounceTime = 0;
bool blinkLedstate = true;
bool state2 = true;
bool buttonState = true;       // Variable to hold the button state
bool lastButtonState = true;   // Variable to track the previous button state
bool store = false; // Stores button state

void yellowLight() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(yellow, HIGH);  // Turn on the LED
    delay(60);  // Delay for 60 milliseconds
    digitalWrite(yellow, LOW);  // Turn off the LED
    delay(60);  // Delay for 60 milliseconds
  }
}
void blueLight() {
//  Turns on the Blue Led
  digitalWrite(blue, HIGH);
}
void redLight() {
//  Turns on the Red led
  digitalWrite(red, HIGH);
  blueLightOff();
}
void blueLightOff() {
//  Turns off Blue Led
  digitalWrite(blue, LOW);
}
void amberLight() {
//  Turns on Amber LED
  digitalWrite(amber, HIGH);
}
void writeFile(fs::FS &fs, const char * path, const char * message) {
//  Writes data into a file
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    redLight();
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
    yellowLight();
  } else {
    Serial.println("Write failed. Red light on");
    redLight();
    while (true);;
  }
  file.close();
}

bool appendFile(fs::FS &fs, const char * path, const char * message) {
  //  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    redLight();
    return false;
  }
  if (file.print(message)) {
    return true;
  } else {
    Serial.println("Append failed");
    redLight();
    return false;
  }
  file.close();
}
void testFileIO(fs::FS &fs, const char * path) {
//  Not necessary to be removed later
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
    redLight();
  }
  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    redLight();
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  yellowLight();
  file.close();
}
void setupLed() {
  pinMode(yellow, OUTPUT);
  pinMode(amber, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(btn, INPUT_PULLUP);
  digitalWrite(yellow, LOW);
  digitalWrite(amber, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);
}
void blinkLed() {
  unsigned long currentMillis = millis();  // Get the current time
  // Check if the specified interval has passed
  if (currentMillis - previousMillis >= BLINK_INTERVAL2) {
    previousMillis = currentMillis;  // Update the previous time
    blinkLedstate = !blinkLedstate;
    // Toggle the LED state
    digitalWrite(blue, blinkLedstate);
  }
}
void setUpMPU() {
  // MPU6050 Tests start here
  Serial.println("MPU6050 test!");
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("No MPU6050");
    redLight();
    while (true) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  blueLight();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  delay(100);
  // MPU code ends here
}
void setUpBMP() {
  //BMP280 Tests start here
  Serial.println("BMP280 test");
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 not found");
    redLight();
    while (true) delay(10); // Infinite loop continues indefinitely
  }
  blueLight();
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  // BMP280 Tests end here
}
void setUpSD() {
  // SD code starts here
  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    redLight();
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  blueLight();
}
int checkFileContent() {
  // Open the file
  File file = SD.open("/data.csv");
  if (file) {
    // Read the first line from the file
    String firstLine = file.readStringUntil('\n');
    file.close();
    // Check if the first line contains the desired string
    if (firstLine.indexOf("id,Time (ms),Alt (m)") != -1) {
      Serial.println("String found!");
      return 1;
    } else {
      Serial.println("String not found!");
      return 0;
    }
  } else {
    Serial.println("Error opening file.");
    return -1;
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while ( !Serial ) delay(100);   // wait for native usb
  setupLed();
  setUpMPU();
  setUpBMP();
  setUpSD();
  writeFile(SD, "/hello.txt", "Hello ");
  appendFile(SD, "/hello.txt", "Gerald It works successfully!\n");
  testFileIO(SD, "/test.txt");
  String dataToHeader = "";
  if (checkFileContent() != 1) {
    dataToHeader = String("id") + "," + String("Time (ms)") + "," + String("Alt (m)") + "," + String("Pres (Pa)")
                   + "," + String("Temp BMP (*C)") + "," + String("Acc X(m/s^2)") + "," + String("Acc Y(m/s^2)")
                   + "," + String("Acc z(m/s^2)") + "," + String("Gyro X(rad/s)") + "," + String("Gyro X(rad/s)") + ","
                   + String("Gyro Z(rad/s)") + "," + String("Gyro Temp(*C)") + "\n";
    writeFile(SD, "/data.csv", dataToHeader.c_str());
  }  else if (checkFileContent() == 1) {
    Serial.println("Already set");
  }
}
void printToSerial(const char* label, float value, const char* suffix = "", bool newLine = false) {
  Serial.print(label);
  Serial.print(value);
  Serial.print(suffix);
  if (newLine) {
    Serial.println();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  updateButtonState();
  if (digitalRead(red)) {
    Serial.println("Red light is on");
    delay(1000);
    return;
  }
  if (store) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    long timer = millis();
    String dataToLog = "";
    // BMP280 software
    float bmpPres = bmp.readPressure();
    float bmpAlt = bmp.readAltitude(1026.25);
    float bmpTemp = bmp.readTemperature();


    // BMP280 Software ends here
    // MPU6050 Software starts here

    float accX = a.acceleration.x;
    float accY = a.acceleration.y;
    float accZ = a.acceleration.z;

    float gX = g.gyro.x;
    float gY = g.gyro.y;
    float gZ = g.gyro.z;

    float gTemp = temp.temperature;
    // MPU code ends here
    dataToLog = String(id++) + "," + String(timer) + "," + String(bmpAlt) + "," + String(bmpPres)
                + "," + String(bmpTemp) + "," + String(accX) + "," + String(accY)
                + "," + String(accZ) + "," + String(gX) + "," + String(gY) + ","
                + String(gZ) + "," + String(gTemp) + "\n";
    /* Print out the values */
    unsigned long currTime = millis();  // Get the current time
    // Check if the specified interval has passed
    if (currTime - prevTime >= 1000) {
      prevTime = currTime;  // Update the previous time
      printToSerial("Temp = ", bmpTemp, " *C, ");
      printToSerial("Pres = ", bmpPres, " Pa, ");
      printToSerial("Alt = ", bmpAlt, " m, ", true);
      printToSerial("Ax = ", accX, ", ");
      printToSerial("Ay = ", accY, ", ");
      printToSerial("Az = ", accZ, " m/s^2", true);

      printToSerial("Gx = ", gX, ", ");
      printToSerial("Gy = ", gY, ", ");
      printToSerial("Gz = ", gZ, " rad/s", true);

      printToSerial("Temp AG = ", gTemp, " *C", true);
      prevTime = currTime;
    }

    // SD Card code Starts here
    appendFile(SD, "/data.csv", dataToLog.c_str());
    // SD card code Ends here
    blinkLed();
    //  delay(1000); // To be removed in favour of millis()
  }
}
