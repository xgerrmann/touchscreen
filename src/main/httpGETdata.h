// ## ESP settings for Wifi and GET request
char ssid[]		= "Reetro 21";            // your network SSID (name)
char pass[]		= "JKHj5igu";        // your network password

char server[]	= "api.reetro21.nl";
byte serverPORT	= 80;

// Reference to hardwareserial
HardwareSerial& esp = Serial1;

//StaticJsonBuffer<1024> jsonBuffer;
DynamicJsonBuffer jsonBuffer;

void reset(){
	esp.println("AT+RST");
	delay(1000);
	if(esp.find("OK")) Serial.println("Module Reset");
}

int connectWifi(char* ssid, char* password) {
	String cmd = "AT+CWJAP=\"" +String(ssid)+"\",\"" + String(password) + "\"";
	Serial.println("SSID:.... "+String(ssid)+"\r\nPassword: "+String(password));
	esp.println(cmd);
	Serial.print("Connecting to Wifi");
	int count =0;
	while(!esp.find("OK")){
		Serial.write('.');
		count++;
		if(count > 10) return 1;
	};
	Serial.write('\r');
	Serial.write('\n');
	Serial.println("Connected!");
	return 0;
}

void httpGET(String uri, char* response)
{
//	uri: e.g "/products"
	esp.println("AT+CIPSTART=\"TCP\",\"" + String(server) + "\"," + String(serverPORT));
	Serial.print("Setting up TCP connection");
	while( !esp.find("OK")) {
		Serial.write(".");
	}
	delay(1000);
	Serial.println("\r\nTCP connection ready\r\n");
	String request =
	"GET " + uri + " HTTP/0.9\r\n" +
	"Host: " + server + "\r\n"
	"Authorization: Basic eGFuZGVyQGdlcnJtYW5uLm5sOmxhc2VycGljbw==\r\n\r\n";
	Serial.println("Send GET request:\r\n----------");
	Serial.print(request);
	Serial.println("----------");
	String sendCMD = "AT+CIPSEND=";
	esp.print(sendCMD);
	esp.println(request.length());
	if(esp.find(">"))
	{
		Serial.println("Sending");
		esp.print(request);
		if( esp.find("SEND OK"))
		{
			int length = 0;
			while( esp.find('+IPD,')){
				length = esp.parseInt(); esp.find(':'); // get length and remove ':'
				char message[length];
				esp.readBytesUntil('?',message,length);
				strncat(response,message,length);
			}
		}
	}
	Serial.println("Close connection");
	esp.println("AT+CIPCLOSE");
}

getProducts()
{
	Serial.println("Retrieving products");
	const char response[4000] = "";
	httpGET("/products",response);
	char* json = strchr(response,'[');
	Serial.println(json);
	
	Serial.println("Received (stripped) Json:");
	Serial.println(json);
	JsonArray& productArray = jsonBuffer.parseArray(json);
	if (!productArray.success())
	{
		Serial.println("parseObject() failed");
		return;
	}
	productArray.prettyPrintTo(Serial);

	// Place in Products
	Serial.println("\r\nProducts found:");
	for(JsonArray::iterator it1=productArray.begin(); it1!=productArray.end(); ++it1)
	{
		JsonObject& prod= *it1;
		int id			= NULL;
		char name[20]	= "";
		for(JsonObject::iterator it2=prod.begin(); it2!=prod.end(); ++it2)
		{
			if(strcmp(it2->key,"id")==0)
			{
				id = it2->value.as<int>();
			} else if(strcmp(it2->key,"name")==0) {
				strcpy(name,it2->value);
			} else {
				Serial.print("No key match");
			}
		}
		Serial.println("Id: "+String(id)+", name: "+String(name));
		products.add(new Product(name,id));
	}
	// TODO: sort by id
}

getPersons()
{
	const char response[4000] = "";
	httpGET("/housemates",response);
	char* json = strchr(response,'[');
	Serial.println(json);
	
	Serial.println("Received (stripped) Json:");
	Serial.println(json);
	JsonArray& productArray = jsonBuffer.parseArray(json);
	if (!productArray.success())
	{
		Serial.println("parseObject() failed");
		return;
	}
	productArray.prettyPrintTo(Serial);

	// Place in Persons
	Serial.println("\r\nPersons found:");
	for(JsonArray::iterator it1=productArray.begin(); it1!=productArray.end(); ++it1)
	{
		JsonObject& pers= *it1;
		int id			= NULL;
		char name[20]	= "";
		int quantity	= 0;
		for(JsonObject::iterator it2=pers.begin(); it2!=pers.end(); ++it2)
		{
			if(strcmp(it2->key,"id")==0)					// First element
			{
				id		= it2->value.as<int>();
			} else if(strcmp(it2->key,"quantity")==0) {	// Second element
				quantity= it2->value.as<int>();
			} else if(strcmp(it2->key,"name")==0){			// Last (third) element
				strcpy(name,it2->value);
			} else {
				Serial.print("No key match");
			}
		}
		Serial.println("Id: "+String(id)+", name: "+String(name)+", quantity: "+String(quantity));
		persons.add(new Person(name,id,quantity));
	}
	//TODO: sort by quantity
}
