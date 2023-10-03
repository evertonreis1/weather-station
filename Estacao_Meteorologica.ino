#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2icfqH2P7"
#define BLYNK_TEMPLATE_NAME "Estacao Meteorologica"

#include "WiFi.h"
#include "ESPAsyncWebSrv.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <BlynkSimpleEsp32.h>

// Replace with your network credentials
const char *ssid = "your_ssid";
const char *password = "your_password";
// Blynk authentication token
char auth[] = "GKllOzsEZ2Jhl-BLLukcM0kINGTjmY3B";

#define DHTPIN 5    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

BlynkTimer timer;

void readAndSendSensorData()
{
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, temperature); // Envia os valores para os pinos virtuais correspondentes no Blynk
  Blynk.virtualWrite(V1, humidity);
}

String readDHTTemperature()
{
  float t = dht.readTemperature();
  if (isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else
  {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity()
{
  float h = dht.readHumidity();
  if (isnan(h))
  {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else
  {
    Serial.println(h);
    return String(h);
  }
}

char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=Gabarito:wght@700&display=swap" rel="stylesheet">
  <style>
    body {
      background-color: #edf2f4;
      color: #fff; 
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      flex-direction: column;
      text-align: center;
      margin: 0;
      font-family: Arial, Helvetica, sans-serif;
    }

    h1 {
      font-size: 3.0rem;
      letter-spacing: 0.1em;
      text-transform: uppercase;
      margin-bottom: 20px;
      font-family: 'Gabarito', 'Arial', sans-serif;
      color: #1c1d2b;
    }

    .p-wrapper {
      background-color: #2b2d42;
      border-radius: 20px;
      margin: 20px;
      padding: 20px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      max-width: 600px;
      box-shadow: 10px 10px 10px #8d99ae;
    }

    .p-wrapper i {
      margin-right: 20px;
      color: #fff; 
      font-size: 2.5rem;
    }

    .dht-labels {
      font-size: 2.5rem; 
      color: #fff; 
      margin-right: 10px;
    }

    .units {
      font-size: 2.5rem; 
      color: #fff; 
    }

    /* Alinhamento vertical e centralização */
    .p-wrapper p {
      display: flex;
      align-items: center;
      margin: 0;
    }

    /* Margem interna para os dados */
    .data {
      font-size: 2.5rem; 
      margin-left: 10px; 
    }

    @media (max-width: 768px) {
      .p-wrapper {
        flex-direction: column;
      }
    }
  </style>
</head>
<body>
  <h1>ESP32 DHT Server</h1>
  <div class="p-wrapper">
    <p>
      <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
      <span class="dht-labels">Temperature:</span> 
      <span id="temperature" class="data">%TEMPERATURE%</span>
      <span class="units"><sup class="units">&deg;C</sup></span>
    </p>
  </div>
  <div class="p-wrapper">
    <p>
      <i class="fas fa-tint" style="color:#00add6;"></i> 
      <span class="dht-labels">Humidity:</span>
      <span id="humidity" class="data">%HUMIDITY%</span>
      <span class="units"><sup class="units">&percnt;</sup></span>
    </p>
  </div>
</body>

<script>
  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var temperatureValue = this.responseText.replace('h', ''); 
        document.getElementById("temperature").innerHTML = temperatureValue;
      }
    };
    xhttp.open("GET", "/temperature", true);
    xhttp.send();
  }, 10000 );

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("humidity").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/humidity", true);
    xhttp.send();
  }, 10000 );
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String &var)
{
  if (var == "TEMPERATURE")
  {
    return readDHTTemperature();
  }
  else if (var == "HUMIDITY")
  {
    return readDHTHumidity();
  }
  return String();
}

void setup()
{
  Serial.begin(115200);

  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });

  // Start server
  server.begin();

  // Initialize Blynk
  Blynk.begin(auth, ssid, password);
  timer.setInterval(10000L, readAndSendSensorData); 
}

void loop()
{
  Blynk.run();
  timer.run();
}
