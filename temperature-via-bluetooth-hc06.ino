#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include <dht.h>
#include <semphr.h>

SemaphoreHandle_t xBinarySemaphore;
dht DHT;
int Data;
SoftwareSerial BTserial(10, 9); // RX 10 - TX 9;
#define DHT11_PIN 4



// Declarare Task-uri
void TaskTrimitereDate( void *pvParameters );
void TaskCitireTemperatura( void *pvParameters );

// Globals
char transmitere[20]; // Variabila care trimite datele pe seriala
int temperatureData; // Variabila in care se stocheaza date despre temperatura

void setup() {
  // initialize serial communication at 9600 bits per second:
  xBinarySemaphore = xSemaphoreCreateBinary();
  Serial.begin(9600);
  BTserial.begin(9600);

  
  xTaskCreate(
    TaskTrimitereDate
    ,  "task1"
    ,  128
    ,  NULL
    ,  1
    ,  NULL );
  xTaskCreate(
    TaskCitireTemperatura
    ,  "task2"
    ,  128
    ,  NULL
    ,  1
    ,  NULL );
xSemaphoreGive(xBinarySemaphore);
  vTaskStartScheduler();
  
}

void loop()
{
}

void TaskTrimitereDate(void *pvParameters)  {
  pinMode(7, OUTPUT);
  while (1)
  {xSemaphoreTake(xBinarySemaphore, 5);
    sprintf(transmitere, "Temperatura:%d", temperatureData);
   xSemaphoreGive(xBinarySemaphore);
   vTaskDelay(10);
  
    Serial.write(transmitere);

      Serial.println();
    Serial.println("Waiting for command...");
    
    if (BTserial.available()) { //wait for data received
      Data = BTserial.read();
      if (Data == '1') {
        BTserial.print("Temperatura este: ");
           xSemaphoreTake(xBinarySemaphore, 5);
        BTserial.print(temperatureData);
           xSemaphoreGive(xBinarySemaphore);
            vTaskDelay(10);
        BTserial.println();
 
      }
      else if (Data == '2') {
        
        digitalWrite(7, HIGH);
        BTserial.print("LED Verde Aprins ");
        Serial.println("LED Verde Aprins ");
        
      }
      else if (Data == '3') {
    
        digitalWrite(6, HIGH);
        BTserial.print("LED Rosu Aprins ");
        Serial.println("LED Rosu Aprins ");
  
      }
    else if (Data == '4') {
      
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      BTserial.print("LED-uri stinse ");
      Serial.println("LED-uri stinse ");
    
   
     }
    
    }
      vTaskDelay(500 / portTICK_PERIOD_MS );
  }
}

void TaskCitireTemperatura(void *pvParameters)  {
  while (1)
  { 
    vTaskDelay(1500 / portTICK_PERIOD_MS );
    int chk = DHT.read11(DHT11_PIN);
    xSemaphoreTake(xBinarySemaphore, 5);
    temperatureData = DHT.temperature; // citeste valoarea de temperatura de pe senzor
  
      xSemaphoreGive(xBinarySemaphore);
      vTaskDelay(10);
  }

}
