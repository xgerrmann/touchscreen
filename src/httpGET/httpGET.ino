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
WiFiEspClient client;

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
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, serverPORT)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /open/products HTTP/1.1");
    client.print("Host: "); client.println(server);
    client.println("Connection: keep-alive");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
  }
}




void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {    char c = client.read();    Serial.write(c);  }

  //// if the server's disconnected, stop the client:
 // if (!client.connected()) {
 //   Serial.println();
 //   Serial.println("disconnecting from server.");
 //   client.stop();


   // Terminal(); // nice way to finish

  //}
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void Terminal()
{
  Serial.println("Terminal Mode Forever");

 while(1)
{ 
  if(Serial1.available())  // check if the ESP is sending a message
  {
    while(Serial1.available())
    {
      char c = Serial1.read();  // read the next character.
      Serial.write(c);  // writes data to the serial monitor
    }
  }

  if(Serial.available())
  {
    delay(10);  // wait to let all the input command in the serial buffer

    // read the input command in a string
    String cmd = "";
    while(Serial.available())
    {
      cmd += (char)Serial.read();
    }

    // print the command and send it to the ESP
    Serial.println("---------------------");
    Serial.print(">> ");
    Serial.println(cmd);
    Serial.println("---------------------");
    Serial1.println(cmd); // send the read character to the esp8266
  }
}
}
