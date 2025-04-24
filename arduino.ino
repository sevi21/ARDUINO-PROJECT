#include <FastLED.h>

#define NUM_LEDS 231
#define DATA_PIN 2
#define BRIGHTNESS 230

CRGB leds[NUM_LEDS];
uint8_t pos = 0;
bool toggle = false;


char selectedProgram = 'a';


uint8_t gHue = 0;  
uint16_t sparklePos = 0;
uint8_t fireIntensity = 90;
uint8_t wavePos = 0;
unsigned long lastButtonPress = 0;
bool autoMode = false;
unsigned long programChangeTime = 0;

CRGBPalette16 currentPalette;
TBlendType currentBlending = LINEARBLEND;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  
  
  startupAnimation();
  
  Serial.println("LED Controller Ready!");
  Serial.println("Available programs:");
  Serial.println("a - Moving dot with blur");
  Serial.println("b - Rainbow palette");
  Serial.println("c - All LEDs off");
  Serial.println("d - Confetti effect");
  Serial.println("e - Fire effect");
  Serial.println("f - Wave patterns");
  Serial.println("g - Sparkles");
  Serial.println("h - Pulse effect");
  Serial.println("i - Rainbow twinkles");
  Serial.println("j - Police lights"); 
  Serial.println("k - Romantic mood");
  Serial.println("x - Auto mode (cycles through all programs)");
}

void loop() {
  
  if (Serial.available() > 0) {
    char input = Serial.read();
    if ((input >= 'a' && input <= 'k') || input == 'x') {
      selectedProgram = input;
      Serial.print("Program spremenjen na: ");
      Serial.println(selectedProgram);
      
      if (input == 'x') {
        autoMode = true;
        programChangeTime = millis();
        Serial.println("Auto mode enabled - cycling through all programs");
      } else {
        autoMode = false;
      }
      
      lastButtonPress = millis();
    }
  }


  if (autoMode && millis() - programChangeTime > 10000) { 
    programChangeTime = millis();
    selectedProgram++;
    if (selectedProgram > 'j' || selectedProgram < 'a') {
      selectedProgram = 'a';
    }
    Serial.print("Auto switching to program: ");
    Serial.println(selectedProgram);
  }

  
  switch (selectedProgram) {
    case 'a': programA(); break;
    case 'b': programB(); break;
    case 'c': programC(); break;
    case 'd': programD(); break;
    case 'e': programE(); break;
    case 'f': programF(); break;
    case 'g': programG(); break;
    case 'h': programH(); break;
    case 'i': programI(); break;
    case 'j': programJ(); break;
    case 'k': programK(); break;
    default: programA(); break;
  }
  
 
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

void startupAnimation() {
 
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(i, 255, 255);
    FastLED.show();
    delay(10);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(5);
  }
}

void programA() {
  
  leds[pos] = CHSV(pos * 2, 255, 255);
  blur1d(leds, NUM_LEDS, 172);
  fadeToBlackBy(leds, NUM_LEDS, 16);
  FastLED.show();

  if (toggle) {
    pos = (pos + 1) % NUM_LEDS;
  }
  toggle = !toggle;
  delay(20);
}

void programB() {
  
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;

  FillLEDsFromPaletteColors(startIndex);
  FastLED.show();
  FastLED.delay(1000 / 100);
}

void programC() {
  
  FastLED.clear(); 
  FastLED.show();  
  delay(50);
}

void programD() {
 
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200 + random8(55), 255);
  
 
  pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200 + random8(55), 255);
  
  FastLED.show();
  delay(15);
}

void programE() {

  static byte heat[NUM_LEDS];
  

  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((fireIntensity * 10) / NUM_LEDS) + 2));
  }
  
 
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  

  if (random8() < 120) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }
  
 
  for (int j = 0; j < NUM_LEDS; j++) {
    CRGB color = HeatColor(heat[j]);
    leds[j] = color;
  }
  
  FastLED.show();
  delay(20);
}

void programF() {
  
  fadeToBlackBy(leds, NUM_LEDS, 20);
  
  uint8_t waveSpeed = 5;
  static uint8_t hue = 0;
  
  
  for (int i = 0; i < NUM_LEDS; i++) {
 
    uint8_t wave1 = sin8(i * 8 + wavePos);
    
   
    uint8_t wave2 = sin8(i * 10 + wavePos * 2);
    
  
    uint8_t wave3 = sin8(i * 6 + wavePos / 2);
    
    
    uint8_t combinedWave = (wave1 + wave2 + wave3) / 3;
    
    
    leds[i] = CHSV(hue + i / 2, 255, combinedWave);
  }
  
  wavePos += waveSpeed;
  hue++;
  
  FastLED.show();
  delay(20);
}

void programG() {
  
  fadeToBlackBy(leds, NUM_LEDS, 80);
  
  
  for(int i = 0; i < 10; i++) {
    int pos = random16(NUM_LEDS);
    if (random8() < 80) {
      leds[pos] = CHSV(gHue + random8(30), 200, 255);
    }
  }
  
 
  leds[sparklePos] = CRGB::White;
  sparklePos = (sparklePos + 1) % NUM_LEDS;
  
  FastLED.show();
  delay(20);
}

void programH() {
  
  static uint8_t pulseHue = 0;
  static uint8_t pulseValue = 0;
  static bool increasing = true;
  
  if (increasing) {
    pulseValue += 5;
    if (pulseValue >= 250) increasing = false;
  } else {
    pulseValue -= 5;
    if (pulseValue <= 5) {
      increasing = true;
      pulseHue += 32; 
    }
  }
  
  fill_solid(leds, NUM_LEDS, CHSV(pulseHue, 255, pulseValue));
  
  FastLED.show();
  delay(20);
}

void programI() {

  fadeToBlackBy(leds, NUM_LEDS, 50);
  
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] += CHSV(gHue + i / 2, 255, 50); 
  }
  
 
  for (int i = 0; i < 5; i++) {
    int pos = random16(NUM_LEDS);
    leds[pos] = CHSV(gHue + random8(64), 200, 255);
  }
  
  FastLED.show();
  delay(20);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void programJ() {
  
  static unsigned long lastChange = 0;
  static bool isRed = true;
  static uint8_t flashCount = 0;
  static const uint8_t flashes = 3; 
  

  fadeToBlackBy(leds, NUM_LEDS, 40);
  

  unsigned long currentMillis = millis();
  if (currentMillis - lastChange > 120) {
    lastChange = currentMillis;
    
    
    static uint8_t intensity = 255;
    
    if (intensity == 255) {
      intensity = 0;
      flashCount++;
      
      
      if (flashCount >= flashes) {
        isRed = !isRed;
        flashCount = 0;
      }
    } else {
      intensity = 255;
    }
    
    
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i < NUM_LEDS / 2) {
        
        leds[i] = isRed ? CRGB(intensity, 0, 0) : CRGB(0, 0, intensity);
      } else {
        
        leds[i] = isRed ? CRGB(0, 0, intensity) : CRGB(intensity, 0, 0);
      }
    }
  }
  
  FastLED.show();
  delay(10); 
}


void programK() {
  
  static uint8_t hueBase = 0;  
  static uint8_t pulseValue = 128;
  static int8_t pulseChange = 1;
  static uint16_t sparkleTimer = 0;
  
  
  pulseValue += pulseChange;
  if (pulseValue >= 200 || pulseValue <= 100) {
    pulseChange = -pulseChange;
  }
  

  for (int i = 0; i < NUM_LEDS; i++) {

    uint8_t hue = hueBase + map(sin8(i * 2 + gHue), 0, 255, 0, 10);
    

    uint8_t sat = 255;
    uint8_t val = map(sin8(i + pulseValue), 0, 255, 50, pulseValue);
    
    leds[i] = CHSV(hue, sat, val);
  }
  

  sparkleTimer++;
  if (sparkleTimer > 5) {
    sparkleTimer = 0;
    if (random8() < 30) {  
      int pos = random16(NUM_LEDS);
      leds[pos] = CHSV(15, 130, 255);  
    }
    if (random8() < 15) {  
      int pos = random16(NUM_LEDS);
      leds[pos] = CHSV(0, 0, 255);  
    }
  }
  
  
  uint8_t numSpots = 4;
  for (int i = 0; i < numSpots; i++) {
    int pos = (millis() / 60 + (NUM_LEDS / numSpots * i)) % NUM_LEDS;
    
    // Create a soft glow around each spot
    for (int j = -4; j <= 4; j++) {
      int spotPos = (pos + j + NUM_LEDS) % NUM_LEDS;
      int brightness = 255 - abs(j) * 50;
      if (brightness > 0) {
        // Deep red glow spots
        leds[spotPos] += CHSV(5, 255, brightness); 
      }
    }
  }
  
  
  EVERY_N_SECONDS(15) {
    hueBase = (hueBase + 1) % 10; 
  }
  
  FastLED.show();
  delay(20);
}