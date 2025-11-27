/*
  YMF825 FM Sound Design with Potentiometers

  Control FM synthesis parameters in real-time with 2 potentiometers!

  POT 1 (A0): Select preset sound (0-7)
  POT 2 (A1): Modify FM intensity (feedback/modulation depth)

  Hardware connections:
  YMF825:
    RST_N - Pin 9
    SS    - Pin 10
    MOSI  - Pin 11
    MISO  - Pin 12
    SCK   - Pin 13

  Potentiometers (see BREADBOARD_WIRING.md for details):
    POT 1: Left pin → GND, Middle pin → A0, Right pin → 5V
    POT 2: Left pin → GND, Middle pin → A1, Right pin → 5V

  Play notes on the Serial Monitor or let it auto-play!
*/

#include <SPI.h>

// Power setting: 0=5V, 1=3.3V
#define OUTPUT_power 0

// Potentiometer pins
#define POT_PRESET A0    // Preset selector
#define POT_PARAM A1     // Parameter modifier

// Number of presets
#define NUM_PRESETS 8

// Current settings
int currentPreset = -1;
int currentParam = -1;

// Auto-play settings
bool autoPlay = true;
unsigned long lastNoteTime = 0;
int currentNote = 0;

// ===== LOW-LEVEL SPI FUNCTIONS =====

void set_ss_pin(int val) {
    if(val == HIGH) PORTB |= (4);
    else PORTB &= ~(4);
}

void set_rst_pin(int val) {
    if(val == HIGH) PORTB |= (2);
    else PORTB &= ~(2);
}

void if_write(char addr, unsigned char* data, char num) {
    set_ss_pin(LOW);
    SPI.transfer(addr);
    for(char i = 0; i < num; i++) {
      SPI.transfer(data[i]);
    }
    set_ss_pin(HIGH);
}

void if_s_write(char addr, unsigned char data) {
  if_write(addr, &data, 1);
}

unsigned char if_s_read(char addr) {
    unsigned char rcv;
    set_ss_pin(LOW);
    SPI.transfer(0x80 | addr);
    rcv = SPI.transfer(0x00);
    set_ss_pin(HIGH);
    return rcv;
}

// ===== YMF825 INITIALIZATION =====

void init_825(void) {
   set_rst_pin(LOW);
   delay(1);
   set_rst_pin(HIGH);
   if_s_write(0x1D, OUTPUT_power);
   if_s_write(0x02, 0x0E);
   delay(1);
   if_s_write(0x00, 0x01); // CLKEN
   if_s_write(0x01, 0x00); // ALRST
   if_s_write(0x1A, 0xA3);
   delay(1);
   if_s_write(0x1A, 0x00);
   delay(30);
   if_s_write(0x02, 0x04); // AP1, AP3
   delay(1);
   if_s_write(0x02, 0x00);
   if_s_write(0x19, 0x3F); // MASTER VOL (was 0x20, now MAX for louder output)
   if_s_write(0x1B, 0x3F); // interpolation
   if_s_write(0x14, 0x00); // interpolation
   if_s_write(0x03, 0x03); // Analog Gain (was 0x01, now MAX for louder output)

   if_s_write(0x08, 0xF6);
   delay(21);
   if_s_write(0x08, 0x00);
   if_s_write(0x09, 0xF8);
   if_s_write(0x0A, 0x00);

   if_s_write(0x17, 0x40); // MS_S
   if_s_write(0x18, 0x00);
}

// ===== TONE PRESET TEMPLATES =====
// These are base presets that will be modified by POT 2

// Preset 0: Electric Piano
unsigned char preset_epiano[35] = {
  0x81, 0x01, 0x85,
  0x00, 0x7F, 0xF4, 0xBB, 0x00, 0x10, 0x40,
  0x00, 0xAF, 0xA0, 0x0E, 0x03, 0x10, 0x40,
  0x00, 0x2F, 0xF3, 0x9B, 0x00, 0x20, 0x41,
  0x00, 0xAF, 0xA0, 0x0E, 0x01, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 1: Bright Bell
unsigned char preset_bell[35] = {
  0x81, 0x01, 0x84,
  0x00, 0xFF, 0xFF, 0xAA, 0x00, 0x15, 0x47,
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x11, 0x40,
  0x00, 0xFF, 0xFF, 0x8A, 0x00, 0x18, 0x43,
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 2: Deep Bass
unsigned char preset_bass[35] = {
  0x81, 0x01, 0x80,
  0x00, 0x8F, 0x60, 0xCC, 0x00, 0x01, 0x40,
  0x00, 0x8F, 0x60, 0xCC, 0x00, 0x02, 0x40,
  0x00, 0x5F, 0x50, 0xBB, 0x00, 0x01, 0x40,
  0x00, 0x7F, 0x60, 0x0E, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 3: Organ
unsigned char preset_organ[35] = {
  0x81, 0x01, 0x87,
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x10, 0x40,
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x20, 0x48,
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x30, 0x4A,
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x40, 0x4C,
  0x80, 0x03, 0x81, 0x80
};

// Preset 4: Metallic
unsigned char preset_metal[35] = {
  0x81, 0x01, 0x82,
  0x00, 0xFF, 0xF8, 0x9A, 0x07, 0x11, 0x47,
  0x00, 0xFF, 0xF0, 0x8A, 0x05, 0x13, 0x46,
  0x00, 0xFF, 0xF0, 0x9A, 0x06, 0x17, 0x45,
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 5: Soft Pad
unsigned char preset_pad[35] = {
  0x81, 0x01, 0x83,
  0x00, 0x3F, 0x33, 0xCC, 0x00, 0x11, 0x40,
  0x00, 0x4F, 0x44, 0xCC, 0x02, 0x12, 0x42,
  0x00, 0x3F, 0x33, 0xBB, 0x00, 0x10, 0x40,
  0x00, 0x5F, 0x44, 0x0E, 0x01, 0x10, 0x41,
  0x80, 0x03, 0x81, 0x80
};

// Preset 6: Synth Lead
unsigned char preset_lead[35] = {
  0x81, 0x01, 0x81,
  0x00, 0xFF, 0xF0, 0x9A, 0x03, 0x14, 0x43,
  0x00, 0xFF, 0xF8, 0x8A, 0x02, 0x21, 0x45,
  0x00, 0xFF, 0xF0, 0x9A, 0x00, 0x10, 0x40,
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 7: Plucked String
unsigned char preset_pluck[35] = {
  0x81, 0x01, 0x86,
  0x00, 0xFF, 0xEE, 0x5A, 0x00, 0x11, 0x40,
  0x00, 0xFF, 0xDD, 0x4A, 0x00, 0x12, 0x41,
  0x00, 0xFF, 0xEE, 0x6A, 0x00, 0x20, 0x42,
  0x00, 0xFF, 0xDD, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Array of preset pointers
unsigned char* presets[NUM_PRESETS] = {
  preset_epiano,
  preset_bell,
  preset_bass,
  preset_organ,
  preset_metal,
  preset_pad,
  preset_lead,
  preset_pluck
};

const char* preset_names[NUM_PRESETS] = {
  "Electric Piano",
  "Bright Bell",
  "Deep Bass",
  "Organ",
  "Metallic Clang",
  "Soft Pad",
  "Synth Lead",
  "Plucked String"
};

// Working buffer for modified tone
unsigned char working_tone[35];

// ===== PARAMETER MODIFICATION =====

void applyParameterMod(unsigned char* base_tone, int paramValue) {
  // Copy base preset to working buffer
  memcpy(working_tone, base_tone, 35);

  // Map paramValue (0-1023) to 0-7 for feedback
  int feedback = map(paramValue, 0, 1023, 0, 7);

  // Modify feedback on operators 1 and 3 (bytes 9 and 23)
  // Feedback is in lower 3 bits of these bytes
  working_tone[9] = (working_tone[9] & 0xF8) | feedback;
  working_tone[23] = (working_tone[23] & 0xF8) | feedback;

  // Also modify algorithm slightly based on pot position
  // This creates interesting timbral sweeps
  int algMod = map(paramValue, 0, 1023, 0, 3);
  int baseAlg = base_tone[2] & 0x07;
  int newAlg = (baseAlg + algMod) % 8;
  working_tone[2] = (working_tone[2] & 0xF8) | newAlg;
}

// ===== TONE SETTING =====

void set_tone(unsigned char* tone_data) {
  if_s_write(0x08, 0xF6);
  delay(1);
  if_s_write(0x08, 0x00);
  if_write(0x07, tone_data, 35);
}

// ===== CHANNEL SETUP =====

void set_ch(void) {
   if_s_write(0x0F, 0x30); // keyon = 0
   if_s_write(0x10, 0x7F); // chvol (was 0x71, now MAX for louder output)
   if_s_write(0x11, 0x00); // XVB
   if_s_write(0x12, 0x08); // FRAC
   if_s_write(0x13, 0x00); // FRAC
}

// ===== NOTE CONTROL =====

void keyon(unsigned char fnumh, unsigned char fnuml) {
   if_s_write(0x0B, 0x00); // voice num
   if_s_write(0x0C, 0x7F); // vovol (was 0x54, now MAX for louder output)
   if_s_write(0x0D, fnumh); // fnum
   if_s_write(0x0E, fnuml); // fnum
   if_s_write(0x0F, 0x40); // keyon = 1
}

void keyoff(void) {
   if_s_write(0x0F, 0x00); // keyon = 0
}

// ===== NOTE FREQUENCIES =====

struct Note {
  unsigned char fnum_h;
  unsigned char fnum_l;
};

Note notes[] = {
  {0x14, 0x65},  // C4
  {0x1c, 0x11},  // D4
  {0x1c, 0x42},  // E4
  {0x1c, 0x5d},  // F4
  {0x24, 0x17},  // G4
  {0x24, 0x52},  // A4
  {0x2c, 0x1d},  // B4
  {0x2c, 0x65},  // C5
};

#define NUM_NOTES 8

// ===== POTENTIOMETER READING =====

int readPotWithHysteresis(int pin, int currentValue, int threshold) {
  int raw = analogRead(pin);

  // Add hysteresis to prevent jitter
  if (abs(raw - currentValue) > threshold) {
    return raw;
  }
  return currentValue;
}

void updateParameters() {
  // Read POT 1 - Preset selector
  int pot1 = analogRead(POT_PRESET);
  int newPreset = map(pot1, 0, 1023, 0, NUM_PRESETS - 1);

  // Read POT 2 - Parameter modifier
  int pot2 = readPotWithHysteresis(POT_PARAM, currentParam, 10);

  // Check if preset changed
  bool presetChanged = (newPreset != currentPreset);
  bool paramChanged = (abs(pot2 - currentParam) > 10);

  if (presetChanged || paramChanged) {
    currentPreset = newPreset;
    currentParam = pot2;

    // Apply parameter modification
    applyParameterMod(presets[currentPreset], currentParam);

    // Upload modified tone
    set_tone(working_tone);

    // Print status
    if (presetChanged) {
      Serial.print("\n>>> Preset ");
      Serial.print(currentPreset);
      Serial.print(": ");
      Serial.println(preset_names[currentPreset]);
    }

    if (paramChanged) {
      int fb = map(currentParam, 0, 1023, 0, 7);
      int algMod = map(currentParam, 0, 1023, 0, 3);
      Serial.print("    FM Intensity: ");
      Serial.print(map(currentParam, 0, 1023, 0, 100));
      Serial.print("% (FB=");
      Serial.print(fb);
      Serial.print(", AlgMod=+");
      Serial.print(algMod);
      Serial.println(")");
    }
  }
}

// ===== SETUP =====

void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("YMF825 FM Synth - Potentiometer Control");
  Serial.println("========================================");
  Serial.println("POT 1 (A0): Select preset (0-7)");
  Serial.println("POT 2 (A1): FM intensity (feedback+algorithm)");
  Serial.println("Auto-playing C major scale...\n");

  // Setup pins
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(POT_PRESET, INPUT);
  pinMode(POT_PARAM, INPUT);
  set_ss_pin(HIGH);

  // Initialize SPI
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();

  // Initialize YMF825
  init_825();
  set_ch();

  // Initialize with first preset
  currentParam = analogRead(POT_PARAM);
  currentPreset = 0;
  applyParameterMod(presets[0], currentParam);
  set_tone(working_tone);
}

// ===== MAIN LOOP =====

void loop() {
  // Always check and update parameters
  updateParameters();

  // Auto-play notes
  if (autoPlay && (millis() - lastNoteTime > 500)) {
    Note n = notes[currentNote];
    keyon(n.fnum_h, n.fnum_l);
    delay(300);
    keyoff();

    lastNoteTime = millis();
    currentNote = (currentNote + 1) % NUM_NOTES;
  }

  delay(10); // Small delay for stability
}
