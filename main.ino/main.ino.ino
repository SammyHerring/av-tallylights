#include <M5StickC.h>
#include <WiFi.h>
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>

IPAddress clientIp(192, 168, 3, 100);          // IP address of the ESP32
IPAddress switcherIp(192, 168, 3, 240);       // IP address of the ATEM switcher
ATEMstd AtemSwitcher;

// http://www.barth-dev.de/online/rgb565-color-picker/
#define GRAY  0x4208 //   64  64  64
#define WHITE  0xFFFF //   255  255  255
#define GREEN 0x0200 //   0 128  0
#define RED   0xF800 // 255  0  0
#define BLUE 0x001F // 0 0 255

const char* ssid = "AVMedia";
const char* password =  "doinglifetogether";

int cameraNumber = 5;
int ledPin = 10;

int PreviewTallyPrevious = 1;
int ProgramTallyPrevious = 1;

void setup() {

  Serial.begin(9600);

  // initialize the M5StickC object
  M5.begin();

  // Start the Ethernet, Serial (debugging) and UDP:
  WiFi.begin(ssid, password);

  drawLabel(GRAY, BLUE, HIGH);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  pinMode(ledPin, OUTPUT);  // LED: 1 is on Program (Tally)
  digitalWrite(ledPin, HIGH); // off

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();

  drawLabel(GRAY, BLACK, HIGH);
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  int ProgramTally = AtemSwitcher.getProgramTally(cameraNumber);
  int PreviewTally = AtemSwitcher.getPreviewTally(cameraNumber);

  if ((ProgramTallyPrevious != ProgramTally) || (PreviewTallyPrevious != PreviewTally)) { // changed?

    if ((ProgramTally && !PreviewTally) || (ProgramTally && PreviewTally) ) { // only program, or program AND preview
      drawLabel(RED, BLACK, LOW);
    } else if (PreviewTally && !ProgramTally) { // only preview
      drawLabel(GREEN, BLACK, HIGH);
    } else if (!PreviewTally || !ProgramTally) { // neither
      drawLabel(BLACK, GRAY, HIGH);
    }

  }

  ProgramTallyPrevious = ProgramTally;
  PreviewTallyPrevious = PreviewTally;
}

void drawLabel(unsigned long int screenColor, unsigned long int labelColor, bool ledValue) {
  digitalWrite(ledPin, ledValue);
  M5.Lcd.fillScreen(screenColor);
  M5.Lcd.setTextColor(labelColor, screenColor);
  M5.Lcd.drawString(String(cameraNumber), 15, 40, 8);
}
