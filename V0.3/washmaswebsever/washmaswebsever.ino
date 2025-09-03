#define ANALOG_PIN 34
#define ADC_THRESHOLD 700
#define OFF_TIMEOUT_MS 30000

int adcValue = 0;
unsigned long lastAboveThreshold = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("ADC Value, Status");
}

void loop() {
  // Read raw ADC value
  adcValue = analogRead(ANALOG_PIN);

  // Update timer if above threshold
  if (adcValue > ADC_THRESHOLD) {
    lastAboveThreshold = millis();
  }

  // Determine status value for Serial Plotter
  int statusPlotValue = (millis() - lastAboveThreshold < OFF_TIMEOUT_MS) ? 1 : 0;

  // Print both for Serial Plotter
  Serial.print(adcValue);
  Serial.print(",");
  Serial.println(statusPlotValue);

  delay(100);  // Plot every 100ms
}
