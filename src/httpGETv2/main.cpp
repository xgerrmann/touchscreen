/*
 WiFiEsp example: WebClient



 Circuit: http://yaab-arduino.h on my module without problems.
I'm using: Arduino Uno, Arduino IDE 1.6.7, ESP-01 on firmware 1.3.0
I think it is your ESP or network. What firmware are you usingblogspot.com/2015/03/esp8266-wiring-schemas.html
*/

#include "WiFiEsp.h"

char ssid[] = "Reetro 21";            // your network SSID (name)
char pass[] = "JKHj5igu";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "api.reetro21.nl";
byte serverPORT = 80;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiEspClient* client = new WiFiEspClient();


// Forward declarations
void printWifiStatus();
void Terminal();
String httpGET();
String getHTMLbody(String);

void setup() {
  // initialize serial and wait for port to open:
  Serial.begin(115200);
//  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only

  Serial1.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial1);
//  WiFi.init(9600);  // initialize ESP serial port with default baud rate 9600

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

  }
  Serial.println("Connected to wifi");

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client->connect(server, serverPORT)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client->println("GET /open/products HTTP/0.9");
    client->print("Host: "); client->println(server);
    client->println("Connection: keep-alive");
    client->println("Connection: close");
    client->println();
  }
}

void loop()
{
	String GETresponse(httpGET());
	Serial.print(GETresponse);
	//String HTMLbody = getHTMLbody(GETresponse);
	//Serial.print(HTMLbody);
  //// if the server's disconnected, stop the client:
 // if (!client.connected()) {
 //   Serial.println();
 //   Serial.println("disconnecting from server.");
 //   client.stop();


   // Terminal(); // nice way to finish

  //}
}

String httpGET(){
	// if there are incoming bytes available
	// from the server, read them and print them:
	char char_prev	= NULL;
	bool header		= true;
	String body		= "";
	while (client->available())
	{
		char c = client->read();
		if(header==true)
		{
			if(c =='\r' && char_prev == '\n')
			{
				header = false;
				client->read(); // flush one line feed
			}
			char_prev = c;
		}else{
			//Serial.write(c);
			body = body + String(c);
		}
	}
	return body;
}

String getHTMLbody(String GETresponse){
	return GETresponse;
}
