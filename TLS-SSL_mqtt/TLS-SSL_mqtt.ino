#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

//variaveis para ultrasonic switch
#define echoPin 32
#define trigPin 33

//reed switch
#define reedPin 27

//INTERNETS PARA LIGAR ESP32
//char ssid[] = "Vodafone-8EF113";       // your network SSID (name)
//char password[] = "6jPhR79ccnVaTWHj";  // your network password
//char ssid[] = "Estudios  São Pedro2G";       // your network SSID (name)
//char password[] = "Saopedro";  // your network password
//char ssid[] = "A tua mae";        // your network SSID (name)
//char password[] = "apasse12345";  // your network password
//char ssid[] = "O Teu pai";       // your network SSID (name)
//char password[] = "gandatouro";  // your network password
char ssid[] = "IoT-Test";       // your network SSID (name)
char password[] = "Denohd0dkooz8Oir";  // your network password

//MQTT topics
const char *mqtt_topic_cheio = "contentor/cheio";
const char *mqtt_topic_vazio = "contentor/vazio";
const char *mqtt_topic_lotacao = "contentor/lotacao";
const char *mqtt_topic_gas = "contentor/gas";


//intervalo de 30 minutos para medicao
unsigned long previousMillis = 0;  // Store the last time the action was executed
//const long interval = 30 * 60 * 1000;  // Interval in milliseconds (30 minutes)
const long interval = 5000;  // Interval in milliseconds (5 seconds)


// MQTT Broker settings
const char *mqtt_broker = "y29efdb1.ala.us-east-1.emqxsl.com";
const char *mqtt_topic = "emqx/esp32";
const char *mqtt_username = "arduino";
const char *mqtt_password = "arduino1234";
const int mqtt_port = 8084;

// WiFi and MQTT client initialization
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

// Function Declarations
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

  // Set Root CA certificate
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
      mqtt_client.publish(mqtt_topic, "I'm connected to the mother ship");  // Publish message upon connection
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
  /* String disp = String(distance);
  delay(1000); */
  return distance;

  /*Serial.print("Distancia: ");
  Serial.print(disp);
  Serial.println(" cm");*/
}

void loop() {
  if (!mqtt_client.connected()) {
    connectToMQTT();
  } else {
    //codigo que acontece a um intervalo de tempo definido
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      int tampa = digitalRead(reedPin);  // Read the state of the switch
      if (tampa == LOW) {
        Serial.print("Tampa fechada chefe");

        //chamar a função que calcula a lotacao
        medicao();

        //Chamar a funcao que calcula o volume de gases nocivos(metano e monoxido de carbono)
        gasAnaliser();


      } else {
        mqtt_client.publish(mqtt_topic_lotacao, "Tampa Aberta");
        Serial.println("A tampa está aberta chefe");
        Serial.println("Tentar novamente dentro de 5s");
        //delay(5000);
      }
      previousMillis = currentMillis;
    }
  }
  mqtt_client.loop();
}


void medicao() {
  calculate_distance();
  Serial.print("Distancia: ");
  Serial.println(distance);
  if (distance <= 12.87) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 100%");
  }
  if (distance >= 12.87 && distance <= 25.75) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 92%");
  }
  if (distance >= 25.75 && distance <= 38.62) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 75%");
  }
  if (distance >= 38.62 && distance <= 51.50) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 63%");
  }
  if (distance >= 51.50 && distance <= 64.37) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 50%");
  }
  if (distance >= 64.37 && distance <= 77.25) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 38%");
  }
  if (distance >= 77.25 && distance <= 90.12) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 25%");
  }
  if (distance >= 90.12 && distance <= 97) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 12%");
  }
  if (distance >= 97) {
    mqtt_client.publish(mqtt_topic_lotacao, "Lotação: 0%");
  }
}


void gasAnaliser() {
  float sensor_volt = 0.0;  // Initialize sensor_volt
  float RS_air = 0.0;       // Initialize RS_air
  float R0 = 0.0;           // Initialize R0
  float sensorValue = 0.0;  // Initialize sensorValue

  //Average
  for (int x = 0; x < 100; x++) {
    sensorValue = sensorValue + analogRead(34);
    delay(10);  // Add a small delay between analog readings
  }
  sensorValue = sensorValue / 100.0;
  //-----------------------------------------------/

  sensor_volt = (sensorValue / 4095.0) * 3.3;  // Adjust conversion for 12-bit ADC and 3.3V reference
  RS_air = (3.3 - sensor_volt) / sensor_volt;  // Depend on RL on your module
  R0 = RS_air / 9.9;                           // According to MQ9 datasheet table

  Serial.print("sensor_volt = ");
  Serial.print(sensor_volt);
  Serial.println("V");

  Serial.print("R0 = ");
  Serial.println(R0);
}