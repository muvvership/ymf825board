/*
  YMF825 FM Sound Design Experiments

  This sketch demonstrates different FM synthesis tones using various:
  - Algorithms (8 types - different operator routing)
  - Waveforms (29 types - different harmonic content)
  - ADSR envelopes (Attack, Decay, Sustain, Release)
  - Feedback and modulation settings

  Hardware connections:
  RST_N - Pin 9
  SS    - Pin 10
  MOSI  - Pin 11
  MISO  - Pin 12
  SCK   - Pin 13

  Press the button or wait to cycle through different preset sounds!
*/

#include <SPI.h>

// Power setting: 0=5V, 1=3.3V
#define OUTPUT_power 0

// Number of preset tones
#define NUM_PRESETS 8

int currentPreset = 0;

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

// ===== TONE PRESET DEFINITIONS =====

/*
  FM SYNTHESIS QUICK REFERENCE:

  Algorithms (ALG 0-7): How operators connect
  - ALG 0: All in series (most harmonic)
  - ALG 7: All parallel (additive, like organ)

  Waveforms (WS 0-28): Shape of oscillator
  - 0: Sine (pure tone)
  - 1-28: Various harmonic waveforms

  Envelope (AR/DR/SR/RR): Volume over time
  - AR: Attack Rate (0-15, higher = faster)
  - DR: Decay Rate
  - SR: Sustain Rate
  - RR: Release Rate

  TL: Total Level (0-63, lower = louder)
  FB: Feedback (0-7, adds harmonics)
  MULTI: Frequency multiplier (0-15)
*/

// Preset 0: Classic FM Electric Piano (Algorithm 5, mixed waveforms)
unsigned char preset_epiano[35] = {
  0x81,  // Header: Tone 0
  // Entire tone settings
  0x01, 0x85,  // BO=1, LFO=0, ALG=5
  // Operator 1 (Modulator)
  0x00, 0x7F, 0xF4, 0xBB, 0x00, 0x10, 0x40,
  // Operator 2 (Carrier)
  0x00, 0xAF, 0xA0, 0x0E, 0x03, 0x10, 0x40,
  // Operator 3 (Modulator)
  0x00, 0x2F, 0xF3, 0x9B, 0x00, 0x20, 0x41,
  // Operator 4 (Carrier)
  0x00, 0xAF, 0xA0, 0x0E, 0x01, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80  // End marker
};

// Preset 1: Bright Bell (Algorithm 4, fast attack)
unsigned char preset_bell[35] = {
  0x81,
  0x01, 0x84,  // ALG=4
  // Op1: Fast attack, bright waveform
  0x00, 0xFF, 0xFF, 0xAA, 0x00, 0x15, 0x47,
  // Op2: Carrier
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x11, 0x40,
  // Op3: High frequency modulator
  0x00, 0xFF, 0xFF, 0x8A, 0x00, 0x18, 0x43,
  // Op4: Carrier
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 2: Deep Bass (Algorithm 0, series modulators)
unsigned char preset_bass[35] = {
  0x81,
  0x01, 0x80,  // ALG=0 (all series)
  // Op1: Slow attack
  0x00, 0x8F, 0x60, 0xCC, 0x00, 0x01, 0x40,
  // Op2
  0x00, 0x8F, 0x60, 0xCC, 0x00, 0x02, 0x40,
  // Op3
  0x00, 0x5F, 0x50, 0xBB, 0x00, 0x01, 0x40,
  // Op4: Final carrier
  0x00, 0x7F, 0x60, 0x0E, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 3: Organ-like (Algorithm 7, all parallel)
unsigned char preset_organ[35] = {
  0x81,
  0x01, 0x87,  // ALG=7 (all parallel - additive)
  // Op1: Fundamental
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x10, 0x40,
  // Op2: 2nd harmonic
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x20, 0x48,
  // Op3: 3rd harmonic
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x30, 0x4A,
  // Op4: 4th harmonic
  0x00, 0xFF, 0xFF, 0x0E, 0x00, 0x40, 0x4C,
  0x80, 0x03, 0x81, 0x80
};

// Preset 4: Metallic Clang (High feedback)
unsigned char preset_metal[35] = {
  0x81,
  0x01, 0x82,  // ALG=2
  // Op1: High feedback creates inharmonic tones
  0x00, 0xFF, 0xF8, 0x9A, 0x07, 0x11, 0x47,
  // Op2
  0x00, 0xFF, 0xF0, 0x8A, 0x05, 0x13, 0x46,
  // Op3
  0x00, 0xFF, 0xF0, 0x9A, 0x06, 0x17, 0x45,
  // Op4
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 5: Soft Pad (Slow attack, algorithm 3)
unsigned char preset_pad[35] = {
  0x81,
  0x01, 0x83,  // ALG=3
  // Op1: Very slow attack
  0x00, 0x3F, 0x33, 0xCC, 0x00, 0x11, 0x40,
  // Op2
  0x00, 0x4F, 0x44, 0xCC, 0x02, 0x12, 0x42,
  // Op3
  0x00, 0x3F, 0x33, 0xBB, 0x00, 0x10, 0x40,
  // Op4
  0x00, 0x5F, 0x44, 0x0E, 0x01, 0x10, 0x41,
  0x80, 0x03, 0x81, 0x80
};

// Preset 6: Bright Synth Lead (Algorithm 1)
unsigned char preset_lead[35] = {
  0x81,
  0x01, 0x81,  // ALG=1
  // Op1: Fast vibrato effect
  0x00, 0xFF, 0xF0, 0x9A, 0x03, 0x14, 0x43,
  // Op2
  0x00, 0xFF, 0xF8, 0x8A, 0x02, 0x21, 0x45,
  // Op3
  0x00, 0xFF, 0xF0, 0x9A, 0x00, 0x10, 0x40,
  // Op4
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Preset 7: Plucked String (Fast attack, fast decay)
unsigned char preset_pluck[35] = {
  0x81,
  0x01, 0x86,  // ALG=6
  // Op1: Very fast attack and decay
  0x00, 0xFF, 0xEE, 0x5A, 0x00, 0x11, 0x40,
  // Op2
  0x00, 0xFF, 0xDD, 0x4A, 0x00, 0x12, 0x41,
  // Op3
  0x00, 0xFF, 0xEE, 0x6A, 0x00, 0x20, 0x42,
  // Op4
  0x00, 0xFF, 0xDD, 0x0A, 0x00, 0x10, 0x40,
  0x80, 0x03, 0x81, 0x80
};

// Array of all presets
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

// ===== TONE SETTING FUNCTION =====

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

// ===== NOTE FREQUENCY TABLE =====
// Format: {fnum_high, fnum_low}
// These are calculated for specific musical notes

struct Note {
  unsigned char fnum_h;
  unsigned char fnum_l;
};

// C major scale
Note note_C4  = {0x14, 0x65};  // Middle C
Note note_D4  = {0x1c, 0x11};
Note note_E4  = {0x1c, 0x42};
Note note_F4  = {0x1c, 0x5d};
Note note_G4  = {0x24, 0x17};
Note note_A4  = {0x24, 0x52};
Note note_B4  = {0x2c, 0x1d};
Note note_C5  = {0x2c, 0x65};

// ===== SETUP =====

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  Serial.println("YMF825 FM Sound Design Experiments");
  Serial.println("==================================");

  // Setup pins
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  set_ss_pin(HIGH);

  // Initialize SPI
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();

  // Initialize YMF825
  init_825();

  // Load first preset
  set_tone(presets[currentPreset]);
  set_ch();

  Serial.print("Preset 0: ");
  Serial.println(preset_names[currentPreset]);
  Serial.println("Playing C major scale...\n");
}

// ===== MAIN LOOP =====

void loop() {
  // Play a C major scale with current preset
  keyon(note_C4.fnum_h, note_C4.fnum_l);
  delay(400);
  keyoff();
  delay(100);

  keyon(note_D4.fnum_h, note_D4.fnum_l);
  delay(400);
  keyoff();
  delay(100);

  keyon(note_E4.fnum_h, note_E4.fnum_l);
  delay(400);
  keyoff();
  delay(100);

  keyon(note_F4.fnum_h, note_F4.fnum_l);
  delay(400);
  keyoff();
  delay(100);

  keyon(note_G4.fnum_h, note_G4.fnum_l);
  delay(400);
  keyoff();
  delay(100);

  keyon(note_A4.fnum_h, note_A4.fnum_l);
  delay(400);
  keyoff();
  delay(100);

  keyon(note_B4.fnum_h, note_B4.fnum_l);
  delay(400);
  keyoff();
  delay(100);

  keyon(note_C5.fnum_h, note_C5.fnum_l);
  delay(600);
  keyoff();
  delay(500);

  // Switch to next preset
  currentPreset = (currentPreset + 1) % NUM_PRESETS;
  set_tone(presets[currentPreset]);

  Serial.print("Preset ");
  Serial.print(currentPreset);
  Serial.print(": ");
  Serial.println(preset_names[currentPreset]);

  delay(1000);
}
