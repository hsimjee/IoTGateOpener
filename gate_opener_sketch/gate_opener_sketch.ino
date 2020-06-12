// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "VitaminZ";
const char* password = "na3na3Mint";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String opened = "no";

// Assign output variables to GPIO pins
const int gateOpener = 2;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output
  pinMode(gateOpener, OUTPUT);
  digitalWrite(gateOpener, HIGH);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
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
            if (header.indexOf("GET /open") >= 0) {
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta http-equiv='refresh' content='0;url=http://75.141.120.169/' /><title>Gate Opener</title><meta http-equiv='Content-Type' content='text/html; charset=UTF-8'/></head>");
              client.println("<body>");
              client.println("<img style='width:550px;height:400px;margin-top:15vh;margin-left:22.5vw;' src='https://i.giphy.com/media/xUA7aLpVxPVEoEPXji/giphy.webp' alt='Gate Opened'>");
              
              Serial.println("Openning");
              digitalWrite(gateOpener, LOW);
              delay(5000);
              digitalWrite(gateOpener,HIGH);
            }
            else{
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><title>Gate Opener</title><meta http-equiv='Content-Type' content='text/html; charset=UTF-8'/></head>");
              client.println("<body>");
              client.println("<a href=\'/open'><img style='width:550px;height:400px;margin-top:15vh;margin-left:22.5vw;cursor:pointer;' src='https://s5.gifyu.com/images/open_sesame.gif' alt='Open Sesame'></a>");
            }
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
}
