#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>       // standard library
#include <WebServer.h>  // standard library
#include "SuperMon.h"   // .h file that stores your html page code
#include <PubSubClient.h>

// here you post web pages to your homes intranet which will make page debugging easier
// as you just need to refresh the browser as opposed to reconnection to the web server
#define USE_INTRANET

// once  you are read to go live these settings are what you client will connect to
#define AP_SSID "TestWebSite"
#define AP_PASS "023456789"

// start your defines for pins for sensors, outputs etc.
#define PIN_LED 2     //On board LED

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "naturliches/voltage";
//const char *mqtt_username = "emqx";
//const char *mqtt_password = "public";
const int mqtt_port = 1883;

// variables to store measure data and sensor states
int BitsA0 = 0, BitsA1 = 0;
float VoltsA0 = 0, VoltsA1 = 0, VoltsA2 = 0, VoltsA3 = 0;

bool buttonState = false;
bool LED0 = false;
uint32_t SensorUpdate = 0;

bool accessPointMode = true;
String uname;
String psw;

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[512];

// just some buffer holder for char operations
char buf[32];

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress Actual_IP;

// definitions of your desired intranet created by the ESP32
IPAddress PageIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

// gotta create a server
WebServer server(80);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(4, INPUT_PULLUP);
  // standard stuff here
  Serial.begin(115200);

  //*****pinMode(PIN_LED, OUTPUT);

  // turn off led
  LED0 = false;
    startAccessPoint();
  
    //--printWifiStatus();

    // these calls will handle data coming back from your web page
    // this one is a page request, upon ESP getting / string the web page will be sent
    server.on("/", SendWebsite);

    // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
    // just parts of the web page
    server.on("/advices", SendAdvices);
    server.on("/xml", SendXML);
    server.on("/BUTTON_0", ProcessButton_0);

    // finally begin the server
    server.begin();

}

void loop()
{
  unsigned long int last_measurement;
  unsigned long int last_click;
  unsigned long int last_wificheck;
  bool publishment = true;
  char buffer[10];  // Buffer to hold the converted string
  bool last_buttonState = false;
  
  last_measurement = millis();
  last_click = millis();

  while (true){
    buttonState = digitalRead(4);
    if (buttonState != last_buttonState){
      if ((millis() - last_click) > 3000){ // check if period between last change is more than 100 ms (debouncing)
        publishment = !publishment;
        //server.sendHeader("Location", "/main");
        //server.send(302, "text/plain", "");

        last_click = millis();
      }
      last_buttonState = buttonState;
    }

    if(publishment){
      if (millis() - last_measurement > 2000){
      BitsA0 = analogRead(A0);
      VoltsA0 = float(BitsA0) / 1024.0;
      Serial.printf("%.2f \t %.2f \n", BitsA0, VoltsA0);
      publishMessage(VoltsA0);

      last_measurement = millis();
      }
    }

    if ((millis() - last_wificheck) > 10000){  // every 10 seconds
      if (accessPointMode){   // If it should be on AP mode
        int a = 1;
        Serial.println("[AccessPoint mode OK]");
      }else{                  // If it should be connected to an external wifi
        if (WiFi.status() != WL_CONNECTED) {
          if (!connectToWiFi(uname, psw)){
            Serial.println("[Stopping WiFi mode]");
            accessPointMode = true;
            //startAccessPoint();
            Serial.println("[Starting AccessPoint mode]");
          }
        }else{
          Serial.println("[WiFi mode OK]");
          if (!client.connected()){
            Serial.println("[Connecting to broker]");
            brokerConnection();
          }else{
            Serial.println("[Broker Connection OK]");
          }
        }
      }
      last_wificheck = millis();
    }

	  server.handleClient();
    client.loop();
    delay(20);
  } 
}

void publishMessage(float n) {
  String message = "Voltage: "; // Your message here
  message.concat(String(n));
  client.publish("/naturliches/mineralwasser", message.c_str()); // Publish the message
}


// now process button_0 press from the web site. Typical applications are the used on the web client can
// turn on / off a light, a fan, disable something etc
void ProcessButton_0() {
  
  //
  LED0 = !LED0;
  //digitalWrite(PIN_LED, LED0);
  Serial.printf("Button 0: %d", LED0); 
  
  // regardless if you want to send stuff back to client or not
  // you must have the send line--as it keeps the page running
  // if you don't want feedback from the MCU--or let the XML manage
  // sending feeback

  // option 1 -- keep page live but dont send any thing
  // here i don't need to send and immediate status, any status
  // like the illumination status will be send in the main XML page update
  // code
  server.send(200, "text/plain", ""); //Send web page

  // option 2 -- keep page live AND send a status
  // if you want to send feed back immediataly
  // note you must have reading code in the java script
  /*
    if (LED0) {
    server.send(200, "text/plain", "1"); //Send web page
    }
    else {
    server.send(200, "text/plain", "0"); //Send web page
    }
  */

}
void SendAdvices() {

  Serial.println("sending advices");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  uname = (server.arg("uname"));
  psw = (server.arg("psw"));

  Serial.println(server.arg("uname"));
  Serial.println(server.arg("psw"));
  
  server.send_P(200, "text/html", PAGE_ADVICES);

  delay(5000);
  accessPointMode = false;

  Serial.println(connectToWiFi(uname, psw));

}

// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {

  Serial.println("Sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send_P(200, "text/html", PAGE_MAIN);

}

// code to send the main web page
// I avoid string data types at all cost hence all the char mainipulation code
void SendXML() {

  // Serial.println("sending xml");
 // replicating to send enough content as XML
  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  sprintf(buf, "<V0>%d.%d</V0>\n", (int) (VoltsA0), abs((int) (VoltsA0 * 10)  - ((int) (VoltsA0) * 10)));
  strcat(XML, buf);
  sprintf(buf, "<V1>%d.%d</V1>\n", (int) (VoltsA1), abs((int) (VoltsA1 * 10)  - ((int) (VoltsA1) * 10)));
  strcat(XML, buf);
  sprintf(buf, "<V2>%d.%d</V2>\n", (int) (VoltsA2), abs((int) (VoltsA2 * 10)  - ((int) (VoltsA2) * 10)));
  strcat(XML, buf);
  sprintf(buf, "<V3>%d.%d</V3>\n", (int) (VoltsA3), abs((int) (VoltsA3 * 10)  - ((int) (VoltsA3) * 10)));
  strcat(XML, buf);

  // show led0 status
  if (LED0) {
    strcat(XML, "<LED>1</LED>\n");
  }
  else {
    strcat(XML, "<LED>0</LED>\n");
  }

  if (buttonState) {
    strcat(XML, "<SWITCH>1</SWITCH>\n");
  }
  else {
    strcat(XML, "<SWITCH>0</SWITCH>\n");
  }

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  //--Serial.println("XML Sent"); //XML

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send_P(200, "text/xml", XML);
}

// I think I got this code from the wifi example
void printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.printf("SSID: %s \n", WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.printf("IP Address: %s \n", ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.printf("signal strength (RSSI): %ld dBm \n", rssi);

  // print where to go in a browser:
  Serial.printf("Open http:// %s \n", ip);
}


void callback(char *topic, byte *payload, unsigned int length) {
    Serial.printf("Message arrived in topic: %s \n", topic);
    //Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println("\n-----------------------");
}

void brokerConnection(){
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  client.setKeepAlive(120); // Set to the desired keep-alive time in seconds
  //reconnect();
  int attempt = 0;

  String client_id = "esp32-client-rafa-";
  client_id += String(WiFi.macAddress());
  Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

  while (!client.connected() && (attempt < 10)) {
    if (client.connect(client_id.c_str())) {
      Serial.println("Public EMQX MQTT broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(1000);
    }
    attempt ++;
  }
  if (attempt == 10) {
    Serial.println("failed");
  }
}

void reconnect() {
  int attempt = 0;

  while ((!client.connected()) && (attempt < 10)) {
    if (client.connect("ArduinoClient")) {
      client.subscribe(topic);
      Serial.print("x ");
    } else {
      delay(1000);
    }
    attempt++;
  }
  Serial.println("!");
}

void startAccessPoint(){
  Serial.println("________ Starting server ________");
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.printf("IP address: %s \n", Actual_IP); 
}

bool connectToWiFi(String uname, String psw) {
  //Connect to WiFi Network
  WiFi.softAPdisconnect(true);

  const char* ssid = uname.c_str();
  const char* password = psw.c_str();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int attempt = 0;

  while ((WiFi.status() != WL_CONNECTED) && (attempt < 10)) {
    attempt++;
    delay(3000);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("WiFi connection FAILED"));
    WiFi.softAPdisconnect(false);
    
    startAccessPoint();
    return false;
  }

  Serial.println(F("WiFi connected!"));
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(F("Setup ready"));
  return true;
}
