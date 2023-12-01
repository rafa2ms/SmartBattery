#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>       // standard library
#include <WebServer.h>  // standard library
#include "SuperMon.h"   // .h file that stores your html page code
#include <PubSubClient.h>
#include <EEPROM.h>
#include <esp32-hal-timer.h>
#include "MyLibrary.h"
// here you post web pages to your homes intranet which will make page debugging easier
// as you just need to refresh the browser as opposed to reconnection to the web server
#define USE_INTRANET

// once  you are read to go live these settings are what you client will connect to
#define AP_SSID "TestWebSite"
#define AP_PASS "023456789"

// start your defines for pins for sensors, outputs etc.
#define PIN_LED 8     //On board LED
#define PIN_BUTTON 10 

#define initi 0
#define WIFI_mode 1
#define AP_mode 2

#define QTD_CELLS 5

unsigned long int last_measurement;
unsigned long int last_click;
unsigned long int last_wificheck;
unsigned long int last_blink;
unsigned long int last_publishment;
unsigned long int last_debuging;

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "naturliches/voltage";
const int mqtt_port = 1883;

const int analogPins[] = {A0, A1, A4, A2};
float batteryVoltage[QTD_CELLS] = {0};

bool LedStatus = false;

int myStates[] = {initi, WIFI_mode, AP_mode};
String myStatesStr[] = {"initi", "WIFI_mode", "AP_mode"};

int state;

status ConnectionStatus;
wifidata credential;

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[512];

// just some buffer holder for char operations
char buf[32];

WiFiClient espClient;
PubSubClient client(espClient);

WebServer server(80);

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress Actual_IP;

// definitions of your desired intranet created by the ESP32
IPAddress PageIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

MLS MLS_ARRAY[4] = {
	MLS(0.001760572, 	 0.008101050),
	MLS(0.003619284,	-0.037782547),
	MLS(0.005498460,	-0.016324718),
	MLS(0.007369870,	-0.041780630)
}; 

volatile int button_pressed = 0;

void isr() {
	button_pressed++;
}

/*
const int timerInterval = 1000;  // Interval in milliseconds

void IRAM_ATTR onTimer() {
  // Code to execute when the timer interrupt occurs
  //Serial.println("Timer interrupt!");
  if(accessPointMode){
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
  }
}*/


void setup() {
	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), isr, FALLING);
	analogReadResolution(12);

	pinMode(PIN_BUTTON, INPUT_PULLUP);
	pinMode(PIN_LED, OUTPUT);
	pinMode(A2, INPUT);

	digitalWrite(PIN_LED, LedStatus);

	Serial.begin(115200);

	state = WIFI_mode;

	if  ((check_EEPROM()) && (!connectToWiFi(credential))) {
		state = AP_mode;
		startAccessPoint();
	}
	startServer();
}

  
void loop(){
	switch(state){
		case WIFI_mode:
			WifiRoutine();
			client.loop();
		break;
		
		case AP_mode:
			AccessPointRoutine();
			server.handleClient();
		break;
	}
    
	if (millis() - last_measurement > 5000){
		calculateVoltage();
		
		last_measurement = millis();
	}
  	/*
  	if (millis() - last_debuging > 9000){

		Serial.printf("Current State: %d | %s \n", state, myStatesStr[state]);    
    		Serial.printf("Connection Status\n" 
                    "\t * AP: %d \n"
                    "\t * WiFi: %d \n"
                    "\t * Broker: %d \n"
                  , ConnectionStatus.ap, ConnectionStatus.wifi, ConnectionStatus.broker);
    
		last_debuging = millis();
	}
  	*/
	delay(20);
}

void AccessPointRoutine(){
	if((millis() - last_blink) > 500){
		digitalWrite(PIN_LED, !digitalRead(PIN_LED));
		last_blink = millis();
	}
}

void WifiRoutine() {
	if(button_pressed > 0){
		if ((millis() - last_click) > 1000){ // check if period between last change is more than 100 ms (debouncing)
			state = AP_mode;
			button_pressed = 0;
			startAccessPoint();

			last_click = millis();
		}
	}
	
	if (millis() - last_publishment > 2000){
		if (!client.connected()){
			brokerConnection();
		}else{
			publishVoltage();
		}
		last_publishment = millis();
	}
	
	if ((millis() - last_wificheck) > 10000){
		if ((WiFi.status() != WL_CONNECTED)&&(!connectToWiFi(credential))) {
			Serial.println("[Stopping WiFi mode]");
            
			startAccessPoint();
			state = AP_mode;
    		}
		last_wificheck = millis();
  	}
}

void calculateVoltage(){
	float temp = 0;
	float referenceVoltage = 0;  // Initial reference voltage
	analogSetAttenuation(ADC_11db);  // Set attenuation for the current analog pin
	
	Serial.println("_____________________________________");
	for (int i = 0; i < 4; i++) {
		//batteryVoltage[i] = analogRead(analogPins[i]) * (referenceVoltage / 4095.0);
		batteryVoltage[i] = (analogRead(analogPins[i]) * MLS_ARRAY[i].A) + MLS_ARRAY[i].B;
		temp = batteryVoltage[i];
		batteryVoltage[i] -= referenceVoltage;
		referenceVoltage = temp;  // Adjust reference for the next measurement

		Serial.printf("\t Cell A%d: %.2f Volts \t bytes: %d\n", i+1 , batteryVoltage[i],analogRead(analogPins[i]));
	}
	Serial.println("_____________________________________");
}


void publishVoltage() {
	String voltageValues = "";
	for (int temp_i = 0; temp_i < QTD_CELLS; temp_i++){
		voltageValues.concat(String(batteryVoltage[temp_i])); //
		voltageValues.concat(String(" / ")); // 
	}

	String message = "Voltage: "; // Your message here
	message.concat(voltageValues);
	client.publish("/naturliches/mineralwasser", message.c_str()); // Publish the message
}

bool check_EEPROM(){
	unsigned int addr = 0;
	int c_ASCII = 0;
	int i;

	EEPROM.begin(sizeof(credential));
	EEPROM.get(addr, credential);

	Serial.printf("SSID: %s \nPSW: %s\n", credential.SSID, credential.PSW);

	String myString = credential.SSID;

	for (i = 0; i < myString.length(); i++) {
		char currentChar = myString[i];

		if (static_cast<unsigned char>(currentChar) <= 127) {
			c_ASCII++;
		} 
	}
	//Serial.printf("i: %d \n",i);
	//Serial.printf("c_ASCII: %d \n",c_ASCII);

	return ((credential.SSID[0] != '\0')&&(c_ASCII == i));
}
 
void startServer(){
	// gotta create a server
	
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

// now process button_0 press from the web site. Typical applications are the used on the web client can
// turn on / off a light, a fan, disable something etc
void ProcessButton_0() {
	//
	LedStatus = !LedStatus;
	digitalWrite(PIN_LED, LedStatus);
	Serial.printf("Button 0: %d", LedStatus); 
	server.send(200, "text/plain", ""); //Send web page
}

void SendAdvices() {
	Serial.println("sending advices");
	// you may have to play with this value, big pages need more porcessing time, and hence
	// a longer timeout that 200 ms
	credential.SSID = (server.arg("uname"));
	credential.PSW  = (server.arg("psw"));

	Serial.printf("SSID: %s \nPSW: %s\n", credential.SSID, credential.PSW);

	for(int i = 0; i < sizeof(credential); EEPROM.write(i++,'\0'));
	EEPROM.commit();

	unsigned int addr = 0;
	EEPROM.put(addr, credential);
	EEPROM.commit();

	server.send_P(200, "text/html", PAGE_ADVICES);

	delay(5000);

	state = WIFI_mode;
	if(!connectToWiFi(credential)){
		startAccessPoint();
		state = AP_mode;
	}
}

// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {
	Serial.println("Sending web page");
	// you may have to play with this value, big pages need more porcessing time, and hence
	// a longer timeout that 200 ms
	server.send_P(200, "text/html", PAGE_MAIN);

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
		ConnectionStatus.broker = false;
		Serial.println("failed");
		return;
	}
	ConnectionStatus.broker = true;
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
	Serial.println("[Starting AccessPoint mode]");
	WiFi.softAP(AP_SSID, AP_PASS);
	delay(100);
	WiFi.softAPConfig(PageIP, gateway, subnet);
	delay(100);
	Actual_IP = WiFi.softAPIP();
	Serial.print("IP address: "); 
	Serial.println(Actual_IP);

	ConnectionStatus.ap = true;
  ConnectionStatus.wifi = false;
  ConnectionStatus.broker = false;
}

bool connectToWiFi(wifidata credential) {
	//Connect to WiFi Network
	WiFi.softAPdisconnect(true);

	String uname = credential.SSID;
	String psw = credential.PSW;

	const char* ssid = uname.c_str();
	const char* password = psw.c_str();

	Serial.println("Connecting to WiFi...");
	WiFi.begin(ssid, password);
	int attempt = 0;

	while ((WiFi.status() != WL_CONNECTED) && (attempt < 10)) {
		attempt++;
		delay(500);
		Serial.print(".");
	}
	Serial.println(" !");

	if (WiFi.status() != WL_CONNECTED) {
		Serial.println(F("WiFi connection FAILED"));
		WiFi.softAPdisconnect(false);

		ConnectionStatus.wifi = false;
		return false;
	}

	Serial.println(F("WiFi connected!"));
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println(F("Setup ready"));

	digitalWrite(PIN_LED, HIGH);

  ConnectionStatus.ap = false;
	ConnectionStatus.wifi = true;

	return true;
}

// code to send the main web page
// I avoid string data types at all cost hence all the char mainipulation code
void SendXML() {
	// Serial.println("sending xml");
	// replicating to send enough content as XML
	strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

	for (int k = 0; k < 4; k ++){
		sprintf(buf, "<V%d>%.2f</V%d>\n", k, batteryVoltage[k], k);
		strcat(XML, buf);
	}

	// show LedStatus status
	if (LedStatus) {
		strcat(XML, "<LED>1</LED>\n");
	}
	else {
		strcat(XML, "<LED>0</LED>\n");
	}

	if (digitalRead(PIN_BUTTON)) {
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
