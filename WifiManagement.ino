#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>
#include <WiFi.h>

int AN0_Result = 0;
float AN0_Voltage = 0;
int last_measurement;

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "rafa2ms_esp32/voltage";
//const char *mqtt_username = "emqx";
//const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // it is a good practice to make sure your code sets wifi mode how you want it.

  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  //wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
      Serial.println(WiFi.localIP());
      Serial.println(WiFi.status());
      //WiFi.getNetworkInfo(uint8_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &RSSI, uint8_t *&BSSID, int32_t &channel);
      
      // =================================================================
      // =================================================================
      client.setServer(mqtt_broker, mqtt_port);
      client.setCallback(callback);

      while (!client.connected()) {
        String client_id = "esp32-client-rafa-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

        if (client.connect(client_id.c_str())) {
          Serial.println("Public EMQX MQTT broker connected");
        } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
        }
      }
      // Publish and subscribe
        client.publish(topic, "Hi, I'm ESP32 ^^");
        client.subscribe(topic);
      // =================================================================
      // =================================================================
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}


void loop()
{
  last_measurement = millis();
  char buffer[10];  // Buffer to hold the converted string

  while (true){
    if (millis() - last_measurement > 2000){
      AN0_Result = analogRead(A0);
      AN0_Voltage = float(AN0_Result) / 1024.0;
      Serial.print(AN0_Result);
      Serial.print("\t");
      Serial.print(AN0_Voltage);
      Serial.println(" V");

      // Convert the float to a string
      dtostrf(AN0_Voltage, 5, 2, buffer);  // 5 is the minimum width, 2 is the number of decimal places
      client.publish(topic, buffer);
      client.publish(topic,"voltage");

      last_measurement = millis();
    }
  } 
}
