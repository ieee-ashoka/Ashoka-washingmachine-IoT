#include <WiFi.h>
#include <WebServer.h>

// SENSOR & TIMEOUT SETTINGS (CONFIGURE HERE) 
#define ANALOG_PIN 34
#define ADC_THRESHOLD 700
#define OFF_TIMEOUT_MS 30000 // 30 seconds

// --Adjust for larger networks--
IPAddress local_IP(192, 168, 1, 180);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// --- WIFI CREDENTIALS ---
const char* ssid = "Local SSID";
const char* password = "Password";

//  GLOBAL VARIABLES.. can we make it local ?
int adcValue = 0;
int statusPlotValue = 0;
unsigned long lastAboveThreshold = 0;

// Create WebServer object on port 80
WebServer server(80);

// HTML for the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Appliance Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; text-align: center; background-color: #f0f2f5; margin: 0; padding: 15px; }
    h1, h2 { color: #1c1e21; }
    .card { background: #fff; padding: 25px; margin: 20px auto; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); max-width: 450px; }
    .status { font-size: 3em; font-weight: bold; padding: 20px; border-radius: 10px; margin-top: 10px; transition: all 0.5s; }
    .status.on { background-color: #42b72a; color: white; box-shadow: 0 0 15px rgba(66, 183, 42, 0.5); }
    .status.off { background-color: #6c757d; color: white; }
    .adc { font-size: 1.5em; color: #555; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>Appliance Monitor</h1>
  
  <div class="card">
    <h2>Current Status</h2>
    <div id="status" class="status off">--</div>
    <div class="adc">ADC Value: <span id="adcValue">--</span></div>
  </div>

<script>
// Function to fetch current data and update status display
function updateStatus() {
  fetch('/data')
    .then(response => response.json())
    .then(data => {
      const statusElement = document.getElementById('status');
      const adcElement = document.getElementById('adcValue');
      
      adcElement.textContent = data.adcValue;
      if (data.status == 1) {
        statusElement.textContent = 'ON';
        statusElement.className = 'status on';
      } else {
        statusElement.textContent = 'OFF';
        statusElement.className = 'status off';
      }
    })
    .catch(error => console.error('Error fetching data:', error));
}

// Initial load and periodic updates
window.onload = function() {
    updateStatus();
    setInterval(updateStatus, 2000); // Update every 2 seconds
};
</script>
</body>
</html>
)rawliteral";






// --- WEB SERVER HANDLER FUNCTIONS ---
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleData() {
  String json = "{";
  json += "\"adcValue\":" + String(adcValue);
  json += ", \"status\":" + String(statusPlotValue);
  json += "}";
  server.send(200, "application/json", json);
}



void setup() {
  Serial.begin(115200);

  // CONNECT TO WIFI
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- WEB SERVER ROUTES ---
  server.on("/", handleRoot);
  server.on("/data", handleData);

  // Start server
  server.begin();
}

void loop() {
  // Check for incoming client requests
  server.handleClient();

  // Read raw ADC value from the configured pin
  adcValue = analogRead(ANALOG_PIN);

  // Update timer if above the configured threshold
  if (adcValue > ADC_THRESHOLD) {
    lastAboveThreshold = millis();
  }

  // Determine status value using the configured timeout
  statusPlotValue = (millis() - lastAboveThreshold < OFF_TIMEOUT_MS) ? 1 : 0;

  delay(10); // A small delay to prevent overwhelming the loop
}
