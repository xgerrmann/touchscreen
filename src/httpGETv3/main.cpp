/*
 WiFiEsp example: WebClient

 Circuit: http://yaab-arduino.h on my module without problems.
I'm using: Arduino Uno, Arduino IDE 1.6.7, ESP-01 on firmware 1.3.0
I think it is your ESP or network. What firmware are you usingblogspot.com/2015/03/esp8266-wiring-schemas.html
*/

#include "WiFiEsp.h"
#include "../../include/ArduinoJson/ArduinoJson.h"

// ## ESP settings
char ssid[]		= "Reetro 21";            // your network SSID (name)
char pass[]		= "JKHj5igu";        // your network password

char server[]	= "api.reetro21.nl";
byte serverPORT	= 80;


// Reference to hardwareserial
HardwareSerial& esp = Serial1;

// Forward declarations
void reset();
void connectWifi(char* ssid, char* password);
String httpGET();
String getJSONstring(String response);


DynamicJsonBuffer jsonBuffer;

void setup() {
	char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
	JsonObject& root = jsonBuffer.parseObject(json);
	// TODO: CXA GUARD error solved by changing cx+11 flax in Makefile
	// initialize serial and wait for port to open:
	Serial.begin(115200);
	esp.begin(115200);
	// Reset module
	reset();
	if (!root.success())
	{
		Serial.println("parseObject() failed");
		return;
	}
	for (JsonObject::iterator it=root.begin(); it!=root.end(); ++it)
	{
		Serial.println(it->key);
		Serial.println(it->value.asString());
	}
}

void loop()
{
	connectWifi(ssid, pass);
	String response = httpGET();
	String body		= getJSONstring(response);
	//Serial.println(body);
	
	char json[body.length()+1];
	body.toCharArray(json,body.length()+1);
	Serial.println(json);
	JsonObject& root = jsonBuffer.parseObject(json);
	if (!root.success())
	{
		Serial.println("parseObject() failed");
		return;
	}
	for (JsonObject::iterator it=root.begin(); it!=root.end(); ++it)
	{
		Serial.println(it->key);
		Serial.println(it->value.asString());
	}

	while(true){};
}

void reset(){
	esp.println("AT+RST");
	delay(1000);
	if(esp.find("OK")) Serial.println("Module Reset");
}

void connectWifi(char* ssid, char* password) {
	String cmd = "AT+CWJAP=\"" +String(ssid)+"\",\"" + String(password) + "\"";
	Serial.println(cmd);
	esp.println(cmd);
	Serial.print("Connecting");
	while(!esp.find("OK")){
		Serial.write('.');
	};
	Serial.write('\r');
	Serial.write('\n');
	Serial.println("Connected!");
}

String httpGET()
{
	String response = "";
	esp.println("AT+CIPSTART=\"TCP\",\"" + String(server) + "\"," + String(serverPORT));
	if( esp.find("OK")) {
	Serial.println("TCP connection ready");
	} delay(1000);
	String uri = "/products";
	String request =
	"GET " + uri + " HTTP/0.9\r\n" +
	"Host: " + server + "\r\n"
	"Authorization: Basic eGFuZGVyQGdlcnJtYW5uLm5sOmxhc2VycGljbw==\r\n\r\n";
	Serial.println("Send GET request.");
	Serial.print(request);
	String sendCMD = "AT+CIPSEND=";
	esp.print(sendCMD);
	esp.println(request.length());
	if(esp.find(">"))
	{
		Serial.println("Sending..");
		esp.print(request);
		if( esp.find("SEND OK"))
		{
			Serial.println("Packet sent");
			while (esp.available()) {
				response += esp.readString();
			}
			Serial.println("Close connection");
			esp.println("AT+CIPCLOSE");
		}
	}
	return response;
}

String getJSONstring(String response)
{
	char c_cur	= "";
	int	ii		= 0;
	for(; ii<response.length(); ii++)
	{
		c_cur = response[ii];
		if(c_cur=='[')
		{
			break;
		}
	}
	return response.substring(ii+1,response.length()-9); // Remove 'CLOSED' and the square brackets []
}
