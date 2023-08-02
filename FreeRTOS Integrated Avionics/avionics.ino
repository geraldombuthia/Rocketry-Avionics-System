#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_MPU6050.h"
#include "FS.h"
#include "SD.h"

// Pin Definitions
#define RED_LED_GPIO 26
#define YELLOW_LED_GPIO 25
#define BLUE_LED_GPIO 32
#define AMBER_LED_GPIO 33
#define BUTTON_GPIO 12

// Task Handles
TaskHandle_t taskCollectDataHandle;
TaskHandle_t taskSerialMonitorHandle;

// File System Mount Point
const char *mountPoint = "/sdcard";

// Function Prototypes
void taskCollectData(void *pvParameters);
void taskSerialMonitor(void *pvParameters);

void setupLed() {
    gpio_pad_select_gpio(RED_LED_GPIO);
    gpio_pad_select_gpio(YELLOW_LED_GPIO);
    gpio_pad_select_gpio(BLUE_LED_GPIO);
    gpio_pad_select_gpio(AMBER_LED_GPIO);

    gpio_set_direction(RED_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(YELLOW_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLUE_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(AMBER_LED_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(RED_LED_GPIO, 0);
    gpio_set_level(YELLOW_LED_GPIO, 0);
    gpio_set_level(BLUE_LED_GPIO, 0);
    gpio_set_level(AMBER_LED_GPIO, 0);
}

void yellowLight() {
    for (int i = 0; i < 5; i++) {
        gpio_set_level(YELLOW_LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(60));
        gpio_set_level(YELLOW_LED_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(60));
    }
}

void blueLight() {
    gpio_set_level(BLUE_LED_GPIO, 1);
}

void redLight() {
    gpio_set_level(RED_LED_GPIO, 1);
    blueLightOff();
}

void blueLightOff() {
    gpio_set_level(BLUE_LED_GPIO, 0);
}

void amberLight() {
    gpio_set_level(AMBER_LED_GPIO, 1);
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
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
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        redLight();
        return;
    }
    if (file.print(message)) {
        // Success
    } else {
        Serial.println("Append failed");
        redLight();
    }
    file.close();
}

void setUpMPU() {
    Adafruit_MPU6050 mpu;
    Serial.println("MPU6050 test!");
    if (!mpu.begin()) {
        Serial.println("No MPU6050");
        redLight();
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    Serial.println("MPU6050 Found!");
    blueLight();
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void setUpBMP() {
    Adafruit_BMP280 bmp; // For I2C
    Serial.println("BMP280 test");
    if (!bmp.begin(0x76)) {
        Serial.println("BMP280 not found");
        redLight();
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    blueLight();
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void setUpSD() {
    if (!SD.begin(SD_CONFIG)) {
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
    File file = SD.open("/data.csv");
    if (file) {
        String firstLine = file.readStringUntil('\n');
        file.close();
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

void collectData()
{
    // Data collection code here
    // ...

    // SD Card writing code here
    // ...
}

void serialMonitor()
{
    // Serial Monitor code here
    // ...

    // Display collected data on Serial Monitor
    // ...
}

void taskCollectData(void *pvParameters)
{
    while (1)
    {
        collectData();

        // Delay or vTaskDelay() to control the task rate
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskSerialMonitor(void *pvParameters)
{
    while (1)
    {
        serialMonitor();

        // Delay or vTaskDelay() to control the task rate
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main()
{
    // Initialization code here
    setupLed();
    setUpMPU();
    setUpBMP();
    setUpSD();

    // Create tasks
    xTaskCreatePinnedToCore(taskCollectData, "CollectDataTask", configMINIMAL_STACK_SIZE * 4, NULL, 1, &taskCollectDataHandle, APP_CPU_NUM);
    xTaskCreatePinnedToCore(taskSerialMonitor, "SerialMonitorTask", configMINIMAL_STACK_SIZE * 2, NULL, 2, &taskSerialMonitorHandle, APP_CPU_NUM);
}
