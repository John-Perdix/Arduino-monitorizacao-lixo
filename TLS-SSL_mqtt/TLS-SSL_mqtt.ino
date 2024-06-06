#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

//variaveis para ultrasonic switch
#define echoPin 32
#define trigPin 33

//reed switch
#define reedPin 27

//sensor de gas
#define gasPin 34

//INTERNETS PARA LIGAR ESP32
//char ssid[] = "Vodafone-8EF113";       // your network SSID (name)
//char password[] = "6jPhR79ccnVaTWHj";  // your network password
char ssid[] = "Estudios  São Pedro2G";  // your network SSID (name)
char password[] = "Saopedro";           // your network password
//char ssid[] = "A tua mae";        // your network SSID (name)
//char password[] = "apasse12345";  // your network password
//char ssid[] = "O Teu pai";       // your network SSID (name)
//char password[] = "gandatouro";  // your network password
//char ssid[] = "IoT-Test";       // your network SSID (name)
//char password[] = "Denohd0dkooz8Oir";  // your network password

//MQTT topics
const char *mqtt_topic_cheio = "contentor/cheio";
const char *mqtt_topic_vazio = "contentor/vazio";
const char *mqtt_topic_lotacao = "contentor/lotacao";
const char *mqtt_topic_gas = "contentor/gas";

//intervalo de 30 minutos para medicao
unsigned long previousMillis = 0;  // Store the last time the action was executed
//const long interval = 30 * 60 * 1000;  // Interval in milliseconds (30 minutes)
const long interval = 5000;  // Interval in milliseconds (5 seconds)

//Settings do broker
const char *mqtt_broker = "y29efdb1.ala.us-east-1.emqxsl.com";
const char *mqtt_topic = "emqx/esp32";
const char *mqtt_username = "arduino";
const char *mqtt_password = "arduino1234";
const int mqtt_port = 8883;

//Iniciar os clientes de WIFI e MQTT
WiFiClientSecure esp_client;
PubSubClient mqtt_client(esp_client);

//Custum certificate
const char *ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";

const char *lat = "40.186243201487606";
const char *lng = "-8.414758649055049";

bool gasDetetado = false;

void connectToWiFi();

void connectToMQTT();

void mqttCallback(char *topic, byte *payload, unsigned int length);


void setup() {
  Serial.begin(115200);
  connectToWiFi();

  //set up pins for ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //pin for reed switch
  pinMode(reedPin, INPUT_PULLUP);

  //pin para sensor de gas
  pinMode(gasPin, INPUT);

  // Definir o certificado CA
  esp_client.setCACert(ca_cert);

  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setKeepAlive(60);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTT();

  mqtt_client.subscribe(mqtt_topic_vazio);
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void connectToMQTT() {
  while (!mqtt_client.connected()) {
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    Serial.printf("Connecting to MQTT Broker as %s...\n", client_id.c_str());
    if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      mqtt_client.subscribe(mqtt_topic);
      mqtt_client.publish(mqtt_topic, "I'm connected to the mother ship");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" Retrying in 5 seconds.");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("\n-----------------------");
}

long duration, distance;
float calculate_distance() {
  digitalWrite(trigPin, LOW);
  delay(2);

  digitalWrite(trigPin, HIGH);
  delay(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;

  return distance;
}

void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTT();
  } else {
    //codigo que acontece a um intervalo de tempo definido
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      int tampa = digitalRead(reedPin);
      if (tampa == LOW) {
        Serial.print("Tampa fechada chefe");

        //chamar a função que calcula a lotacao
        medicao();

        //Chamar a funcao que calcula o volume de gases nocivos(metano e monoxido de carbono)
        gasAnaliser();


      } else {
        //Descomentar para enviar. Nao estava a funcionar e nao sabemos porque. Funcionava antes ¯\_(ツ)_/¯
        /* mqtt_client.publish(mqtt_topic_lotacao, "Tampa Aberta");
        Serial.println("A tampa está aberta chefe");
        Serial.println("Tentar novamente dentro de 5s"); */

        //chamar a função que calcula a lotacao
        medicao();

        gasAnaliser();
      }
      previousMillis = currentMillis;
    }
  }
  mqtt_client.loop();
}

void gasAnaliser() {
  float sensor_volt = 0.0;
  float RS_air = 0.0;
  float R0 = 0.0;
  float sensorValue = 0.0;

  //Average
  for (int x = 0; x < 100; x++) {
    sensorValue = sensorValue + analogRead(34);
    delay(10);
  }
  sensorValue = sensorValue / 100.0;
  //-----------------------------------------------/

  sensor_volt = (sensorValue / 4095.0) * 3.3;
  RS_air = (3.3 - sensor_volt) / sensor_volt;
  R0 = RS_air / 9.9;

  Serial.print("sensor_volt = ");
  Serial.print(sensor_volt);
  Serial.println("V");

  Serial.print("R0 = ");
  Serial.println(R0);

  if (R0 < 0.12 || R0 > 0.32) {
    Serial.print("Desvio significante detetado");
    //mqtt_client.publish(mqtt_topic_gas, "Substância detetada no Ar");
    gasDetetado = true;
  } else {
    gasDetetado = false;
  }
}

void medicao() {
  calculate_distance();
  Serial.print("Distancia: ");
  Serial.println(distance);

  // Cria o objeto em JSON
  StaticJsonDocument<200> doc;
  doc["lat"] = lat;
  doc["lng"] = lng;

  if (distance <= 2) {
    doc["msg"] = "Full";
  }
  if (distance > 2 && distance <= 10) {
    doc["msg"] = "Half-full";
  }
  if (distance > 10) {
    doc["msg"] = "Empty";
  }

  // Define o status do gas
  if (gasDetetado) {
    doc["gas"] = "Detected";
  } else {
    doc["gas"] = "NotDetected";
  }

  // Serializar JSON para string
  char buffer[256];
  size_t n = serializeJson(doc, buffer);

  // Publica a mensagem em JSON
  mqtt_client.publish(mqtt_topic_lotacao, buffer, n);
}