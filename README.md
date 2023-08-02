# Avionics System Documentation

<div style="display: flex; flex-direction: row; flex-wrap: wrap;">
  <figure style="margin-right: 20px; margin-bottom: 20px; flex: 1 1 300px;">
    <img src="/Images/Avionics With Battery.jpeg" alt="" style="width: 100%; height: auto;">
    <figcaption></figcaption>
  </figure>
  <figure style="margin-right: 20px; margin-bottom: 20px; flex: 1 1 300px;">
    <img src="/Images/Avionics Closeup.jpeg" alt="" style="width: 100%; height: auto;">
    <figcaption></figcaption>
  </figure>
  <figure style="flex: 1 1 300px;">
    <img src="/Images/Avionics Rearview.jpeg" alt="" style="width: 100%; height: auto;">
    <figcaption></figcaption>
  </figure>
  <figure style="flex: 1 1 300px;">
    <img src="/Images/Avionics Unplugged.jpeg" alt="" style="width: 100%; height: auto;">
    <figcaption></figcaption>
  </figure>
</div>


This is the documentation for the Avionics System driver code designed to collect data from the MPU6050 and BMP280 sensors using an ESP32 microcontroller and write them into the SD card reader module. The system also features LEDs and a Buzzer, along with a button for user input.

## Table of Contents
- [Introduction](#introduction)
- [System Components](#system)
- [Setup and Configuration](#setup)
- [Libraries](#libraries)
- [Data Collection](#data)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## [Introduction](#introduction)
The Avionics System driver code is written by Gerald Mbuthia. The system collects data from MPU6050 (accelerometer and gyroscope) and BMP280 (barometric pressure and temperature sensor) using an ESP32 microcontroller and stores the data on an SD card as a csv file. It also features LEDs and a Buzzer for visual and audible feedback, and a button for user interaction.

## [System Components](#system)
The Avionics System is built using the following components:

* ESP32 Microcontroller: Used as the main processing unit.
* MPU6050: A 6-DoF accelerometer and gyroscope sensor.
* BMP280: A barometric pressure and temperature sensor.
* SD Card Reader: For storing data collected from the sensors.
* LEDs: Multiple LEDs used for visual indicators.
* Buzzer: Used for providing audible feedback.
* Button: For user input and control.

## [Setup and Configuration](#setup)
Wiring
The Avionics System requires the following connections:

* ESP32 Microcontroller:

    - I2C SDA to SDA pin (usually GPIO 21 on ESP32)
    - I2C SCL to SCL pin (usually GPIO 22 on ESP32)
    - VCC to 3.3V
    - GND to GND

* MPU6050:

    - I2C SDA to SDA pin (same as above)
    - I2C SCL to SCL pin (same as above)
    - VCC to 3.3V
    - GND to GND

* BMP280:

    - I2C SDA to SDA pin (same as above)
    - I2C SCL to SCL pin (same as above)
    - VCC to 3.3V
    - GND to GND

* SD Card Reader:

    - MOSI to GPIO 23
    - MISO to GPIO 19
    - SCK to GPIO 18
    - CS to GPIO 5 (can be changed in code if needed)
    - VCC to 3.3V
    - GND to GND

* LEDs and Buzzer:

    Connect the LEDs and Buzzer to the specified GPIO pins (e.g., red to GPIO 26, yellow to GPIO 25, blue to GPIO 32, amber to GPIO 33, and the Buzzer to the desired GPIO pin).
    Button:

Connect one end of the button to the desired GPIO pin (e.g., GPIO 12) and the other end to GND.

## [Libraries](#libraries)
Make sure to install the required libraries:

- Adafruit_BMP280
- Adafruit_MPU6050
- Adafruit_Sensor
- SD
- SPI
- Wire

## [Data Collection](#data)
The Avionics System collects data from the MPU6050 and BMP280 sensors. The data collected includes:

- Time (ms)
- Altitude (m)
- Pressure (Pa)
- Temperature (BMP280)
- Acceleration (m/s^2) in X, Y, and Z axes
- Gyroscope data (rad/s) in X, Y, and Z axes
- Gyroscope temperature (*C)
- The collected data is stored in a CSV file named "data.csv" on the SD card.

## [Usage](#usage)
1. Connect the Avionics System components as described in the Setup and Configuration section.

2. Upload the provided driver code to your ESP32 microcontroller.

3. Open the Serial Monitor to view the collected data and sensor readings.

4. The LEDs and Buzzer will provide visual and audible feedback.

5. Press the button to start data collection and store it on the SD card.

6. The data will be written to the "data.csv" file on the SD card.

## [Contributing](#contributing)
Contributions to the Avionics System driver code are welcome. If you have any improvements or suggestions, feel free to open an issue or submit a pull request.

## [License](#license)
This Avionics System driver code is licensed under the MIT License. Feel free to use, modify, and distribute it for your own projects.
