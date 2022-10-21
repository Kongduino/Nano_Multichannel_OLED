#include <Multichannel_Gas_GMXXX.h>
#include <ss_oled.h>
#include <DHT.h>

//Constants
#define DHTPIN 2 // what pin we're connected to
#define DHTTYPE DHT22 // DHT22
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino

#define BUTTON 8
bool screenOn = true;
double lastEvent;

SSOLED ssoled;
#define SDA_PIN -1
#define SCL_PIN -1
// no reset pin needed
#define RESET_PIN -1
// let ss_oled find the address of our display
#define OLED_ADDR -1
#define FLIP180 0
#define INVERT 0
// Use the default Wire library
#define USE_HW_I2C 1
GAS_GMXXX<TwoWire> gas;
char g_dispText[256] = {0};
double t0;
#define DELAY_DISPLAY 30000
#define DELAY_OLED_OFF 90000

void setup() {
  int rc;
  rc = oledInit(&ssoled, OLED_128x64, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L); // Standard HW I2C bus at 400Khz
  if (rc != OLED_NOT_FOUND) {
    char *msgs[] = {
      (char *)"SSD1306 @ 0x3C",
      (char *)"SSD1306 @ 0x3D",
      (char *)"SH1106 @ 0x3C",
      (char *)"SH1106 @ 0x3D"
    };
    oledFill(&ssoled, 0, 1);
    oledWriteString(&ssoled, 0, 0, 0, (char *)"OLED found:", FONT_NORMAL, 0, 1);
    oledWriteString(&ssoled, 0, 10, 2, msgs[rc], FONT_NORMAL, 0, 1);
    gas.begin(Wire, 0x08); // use the hardware I2C
    updateDisplay();
  }
  dht.begin();
  pinMode(BUTTON, INPUT);
  lastEvent = millis();
}

void loop() {
  if (millis() - t0 > DELAY_DISPLAY && screenOn) updateDisplay();
  if (millis() - lastEvent > DELAY_OLED_OFF && screenOn) switchOled(false);
  if (digitalRead(BUTTON) == HIGH) {
    while (digitalRead(BUTTON) == HIGH) ;
    // debounce
    switchOled(!screenOn);
  }
}

void switchOled(bool status) {
  screenOn = status;
  oledPower(&ssoled, status);
  lastEvent = millis();
}

void updateDisplay() {
  oledFill(&ssoled, 0, 1);
  oledWriteString(&ssoled, 0, 35, 0, (char *)"AQI", FONT_STRETCHED, 0, 1);
  float c;
  c = gas.getGM502B();
  Serial.print("\nThe concentration of VOC is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  String s = "VOC: " + String(int(c)) + " ppm";
  oledWriteString(&ssoled, 0, 0, 2, (char *)s.c_str(), FONT_NORMAL, 0, 1);
  c = gas.getGM102B();
  Serial.print("The concentration of NO2 is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  s = "NO2: " + String(int(c)) + " ppm";
  oledWriteString(&ssoled, 0, 0, 3, (char *)s.c_str(), FONT_NORMAL, 0, 1);
  c = gas.getGM702B();
  Serial.print("The concentration of CO is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  s = "CO: " + String(int(c)) + " ppm";
  oledWriteString(&ssoled, 0, 0, 4, (char *)s.c_str(), FONT_NORMAL, 0, 1);
  c = gas.getGM302B();
  Serial.print("The concentration of C2H5OH is ");
  if (c >= 0) Serial.print(c);
  else Serial.print("invalid");
  Serial.println(" ppm");
  s = "C2H5OH: " + String(int(c)) + " ppm";
  oledWriteString(&ssoled, 0, 0, 5, (char *)s.c_str(), FONT_NORMAL, 0, 1);

  float hum; //Stores humidity value
  float temp; //Stores temperature value
  float converted = 0.00;
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.print("Celsius = ");
  Serial.print(temp);
  //Print degree symbol
  Serial.write(176);
  Serial.println("C");
  Serial.print("Humidity =");
  Serial.print(hum);
  Serial.println("%");
  s = String(temp) + "C " + String(hum) + "%";
  oledWriteString(&ssoled, 0, 0, 6, (char *)s.c_str(), FONT_NORMAL, 0, 1);
  t0 = millis();
}
