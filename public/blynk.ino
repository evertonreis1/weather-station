#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2icfqH2P7"
#define BLYNK_TEMPLATE_NAME "Estacao Meteorologica"

#include <WiFi.h> // Importar todas as bibliotecas necessárias
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "Arduino.h"
#include "DHT.h"
#include <Wire.h>

float temperature; // Parâmetros
float humidity;

char auth[] = "GKllOzsEZ2Jhl-BLLukcM0kINGTjmY3B"; // Substitua isso pelo seu token de autenticação
char ssid[] = "Maria"; // Substitua isso pelo nome da sua rede Wi-Fi (SSID)
char pass[] = "maria2350"; // Substitua isso pela senha da sua rede Wi-Fi

#define DHTPIN 5 // O sensor DHT está conectado à porta D5
#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22 // DHT 22, AM2302, AM2321
// #define DHTTYPE DHT21 // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE); // Inicializa o sensor DHT
BlynkTimer timer;

void sendSensor() // Função para ler os valores do sensor e enviá-los para o Blynk
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, temperature); // Envia os valores para os pinos virtuais correspondentes no Blynk
  Blynk.virtualWrite(V1, humidity);
}

void setup()
{
  Serial.begin(115200);
  dht.begin();
  delay(1000);
  Blynk.begin(auth, ssid, pass);
  delay(1000);
  timer.setInterval(1000L, sendSensor); // A função sendSensor será executada a cada 1000 milissegundos (1 segundo)
}

void loop()
{
  Blynk.run();
  timer.run();
}