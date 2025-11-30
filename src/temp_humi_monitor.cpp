#include "temp_humi_monitor.h"
DHT20 dht20;
LiquidCrystal_I2C lcd(33,16,2);

void TaskLCD(void *pvParameters)
{

	lcd.begin(); // initialize the lcd to use user defined I2C pins
	lcd.backlight();

	lcd.setCursor(3,0);
	lcd.print("Hello, world!");
	lcd.setCursor(2,1);
	lcd.print("Time is now");

  while (1)
  {
    displayTemperatureAndHumidity(lcd);

  }
}
void displayTemperatureAndHumidity(LiquidCrystal_I2C &lcd) {
  for (int i = 0; i < 5; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: " + String(dht20.getTemperature()) + " C");
    lcd.setCursor(0, 1);
    lcd.print("Humid: " + String(dht20.getHumidity()) + "%");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void temp_humi_monitor(void *pvParameters){

    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();

    while (1){
        /* code */
        
        dht20.read();
        // Reading temperature in Celsius
        float temperature = dht20.getTemperature();
        // Reading humidity
        float humidity = dht20.getHumidity();

        

        // Check if any reads failed and exit early
        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read from DHT sensor!");
            temperature = humidity =  -1;
            //return;
        }

        //Update global variables for temperature and humidity
        glob_temperature = temperature;
        glob_humidity = humidity;

        // Print the results
        
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print("%  Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");
        xTaskCreate(TaskLCD, "Task LCD Display" ,4096  ,NULL  ,2 , NULL);
        vTaskDelay(5000);
    }
    
}