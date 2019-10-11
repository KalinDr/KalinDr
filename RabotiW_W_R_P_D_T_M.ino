
// Load Wi-Fi library
#include <WiFi.h>

//Load LCD library
#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Replace with your network credentials
const char* ssid     = "EmnogoPoDobar";
const char* password = "TovaeRouteraNaKalin";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output15State = "off";
String output2State = "off";
String Temp = "off";
String Moisture = "off";

// Assign output variables to GPIO pins
const int output15 = 15;
const int output2 = 2;
const int input18 = 18;
const int input19 = 19;

  int C = 0;
  int M = 0;
  int in35 = 0;
  int in34 = 0;
  String ip;


void setup() {
   // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output15, OUTPUT);
  pinMode(output2, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output15, LOW);
  digitalWrite(output2, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  ip = WiFi.localIP().toString();
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(ip);
  server.begin();

  lcd.setCursor(0, 0);
  lcd.print("IamHereForPlants");
  lcd.setCursor(0,1);
  lcd.print(ip);
  delay(10000);
  
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(18, INPUT);
  pinMode(19, INPUT);
  
}

void printLCD() {
  lcd.clear();
  // set cursor to first column, first row
  // print message
  lcd.setCursor(0,0);
  lcd.print("P is:");
  lcd.print(output15State);
  lcd.print(" H is:");
  lcd.print(output2State);
    if (in35 != -1 && in34 != -1) {
    lcd.setCursor(0,1);
    lcd.print("V=");
    lcd.print(in35*3.3*16/4095+2);
    lcd.print("  I=");
    lcd.print(in34*3.3*2/4095);
    }else {
      lcd.setCursor(0,1);
      lcd.print(ip);}
  }


void loop() {
  
  if (digitalRead(15) == HIGH) {
    in35 = analogRead(35); 
    in34 = analogRead(34); 
  } else {
    in35 = -1;
    in34 = -1;    
  }
  //Measuring temp
  C = analogRead(18);

  //Measuring moisture
  M = analogRead(19);
  
  
  printLCD();
  delay(1000);
  
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
     char c = client.read();             // read a byte, then
      if (client.available()) {             // if there's bytes to read from the client,
        
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              // turns the GPIOs on and off
              if (header.indexOf("GET /15/on") >= 0) {
                Serial.println("GPIO 15 on");
                output15State = "on";
                digitalWrite(output15, HIGH);
              } else if (header.indexOf("GET /15/off") >= 0) {
                Serial.println("GPIO 15 off");
                output15State = "off";
                digitalWrite(output15, LOW);
    
              } else if (header.indexOf("GET /2/on") >= 0) {
                Serial.println("GPIO 2 on");
                output2State = "on";
                digitalWrite(output2, HIGH);
    
              } else if (header.indexOf("GET /2/off") >= 0) {
                Serial.println("GPIO 2 off");
                output2State = "off";
                digitalWrite(output2, LOW);
    
              }
            }   
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #ec1c24;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Smart Irrigation System</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 15 
            client.println("<p>Water temp C " + Temp + "</p>");
            client.println("<p>Moisture % " + Moisture + "</p>"); 
            client.println("<p>Reley1 Waterpump - State " + output15State + "</p>");
            // If the output15State is off, it displays the ON button       
            if (output15State=="off") {
              client.println("<p><a href=\"/15/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/15/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 2  
            client.println("<p>Reley2 Heather - State " + output2State + "</p>");
            // If the output2State is off, it displays the ON button       
            if (output2State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  
}
