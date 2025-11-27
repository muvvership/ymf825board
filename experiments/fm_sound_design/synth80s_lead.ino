/*
  YMF825 80s Synth Lead - Resolution & Rhythm Control

  Classic 80s synthwave lead with arpeggiator!
  Perfect for Stranger Things vibes, retro gaming, and synthwave.

  POT 1 (A0): Resolution (Note Density)
              - 0-25%:   Chromatic (all 12 notes)
              - 25-50%:  Pentatonic (5-note scale - classic!)
              - 50-75%:  Octaves (wide jumps)
              - 75-100%: Single note (drone/pulse)

  POT 2 (A1): Rhythm (Tempo & Pattern)
              - Left:  Slow tempo, long notes (ambient)
              - Right: Fast tempo, short notes (energetic)

  Hardware:
    YMF825:
      RST_N - Pin 9
      SS    - Pin 10
      MOSI  - Pin 11
      MISO  - Pin 12
      SCK   - Pin 13

    Potentiometers:
      POT 1: GND - A0 - 5V (Resolution)
      POT 2: GND - A1 - 5V (Rhythm)

  Pattern: Plays ascending/descending arpeggios
  Twist pots for different scales and rhythms! 🎹✨
*/

#include <SPI.h>

#define OUTPUT_power 0
#define POT_RESOLUTION A0    // Note density/scale
#define POT_RHYTHM A1        // Tempo/pattern

// Current settings
int currentResolution = -1;
int currentRhythm = -1;
int currentScale = 0;        // 0=chromatic, 1=penta, 2=octaves, 3=single
int currentTempo = 200;      // ms per note

// Arpeggiator state
int arpIndex = 0;
bool arpDirection = true;    // true=up, false=down
unsigned long lastArpTime = 0;

// ===== SPI FUNCTIONS =====

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

// ===== YMF825 INITIALIZATION =====

void init_825(void) {
   set_rst_pin(LOW);
   delay(1);
   set_rst_pin(HIGH);
   if_s_write(0x1D, OUTPUT_power);
   if_s_write(0x02, 0x0E);
   delay(1);
   if_s_write(0x00, 0x01);
   if_s_write(0x01, 0x00);
   if_s_write(0x1A, 0xA3);
   delay(1);
   if_s_write(0x1A, 0x00);
   delay(30);
   if_s_write(0x02, 0x04);
   delay(1);
   if_s_write(0x02, 0x00);
   if_s_write(0x19, 0x3F); // MASTER VOL MAX
   if_s_write(0x1B, 0x3F);
   if_s_write(0x14, 0x00);
   if_s_write(0x03, 0x03); // Analog Gain MAX

   if_s_write(0x08, 0xF6);
   delay(21);
   if_s_write(0x08, 0x00);
   if_s_write(0x09, 0xF8);
   if_s_write(0x0A, 0x00);

   if_s_write(0x17, 0x40);
   if_s_write(0x18, 0x00);
}

// ===== 80s SYNTH LEAD PRESET =====
// Bright, cutting lead sound

unsigned char synth_lead_80s[35] = {
  0x81,
  0x01, 0x87,  // Algorithm 7 (parallel - bright, additive)

  // Operator 1: Fundamental (bright)
  0x00,        // SR=0
  0xFF,        // RR=15, DR=15 (fast envelope)
  0xF8,        // AR=15, SL=8 (very fast attack, high sustain)
  0x0E,        // TL=0 (LOUD)
  0x00,        // No mods
  0x10,        // MULTI=1 (fundamental)
  0x40,        // WS=4, FB=0

  // Operator 2: Octave up (brightness)
  0x00,
  0xFF,        // Fast release/decay
  0xF6,        // Fast attack, medium sustain
  0x2E,        // TL=2 (loud but not max)
  0x00,
  0x20,        // MULTI=2 (octave up)
  0x48,        // WS=4, FB=0

  // Operator 3: Fifth (harmonic richness)
  0x00,
  0xFF,
  0xF5,        // Fast attack
  0x4E,        // TL=4 (medium)
  0x00,
  0x30,        // MULTI=3 (fifth)
  0x4A,        // WS=4, FB=2

  // Operator 4: High harmonic (sizzle)
  0x00,
  0xEF,        // Slightly slower release
  0xE4,        // Medium attack for smoothness
  0x5E,        // TL=5 (quieter, just for color)
  0x00,
  0x40,        // MULTI=4 (two octaves up)
  0x4C,        // WS=4, FB=4

  0x80, 0x03, 0x81, 0x80
};

// ===== TONE UPLOAD =====

void set_tone(unsigned char* tone_data) {
  if_s_write(0x08, 0xF6);
  delay(1);
  if_s_write(0x08, 0x00);
  if_write(0x07, tone_data, 35);
}

// ===== CHANNEL SETUP =====

void set_ch(void) {
   if_s_write(0x0F, 0x30);
   if_s_write(0x10, 0x7F);
   if_s_write(0x11, 0x00);
   if_s_write(0x12, 0x08);
   if_s_write(0x13, 0x00);
}

// ===== NOTE CONTROL =====

void keyon(unsigned char fnumh, unsigned char fnuml) {
   if_s_write(0x0B, 0x00);
   if_s_write(0x0C, 0x7F);
   if_s_write(0x0D, fnumh);
   if_s_write(0x0E, fnuml);
   if_s_write(0x0F, 0x40);
}

void keyoff(void) {
   if_s_write(0x0F, 0x00);
}

// ===== NOTE DEFINITIONS =====

struct Note {
  unsigned char fnum_h;
  unsigned char fnum_l;
  const char* name;
};

// Chromatic scale (C4 to C5)
Note chromatic_scale[] = {
  {0x14, 0x65, "C4"},   // 0
  {0x14, 0x73, "C#4"},  // 1
  {0x1C, 0x11, "D4"},   // 2
  {0x1C, 0x29, "D#4"},  // 3
  {0x1C, 0x42, "E4"},   // 4
  {0x1C, 0x5d, "F4"},   // 5
  {0x1C, 0x79, "F#4"},  // 6
  {0x24, 0x17, "G4"},   // 7
  {0x24, 0x34, "G#4"},  // 8
  {0x24, 0x52, "A4"},   // 9
  {0x24, 0x71, "A#4"},  // 10
  {0x2C, 0x1d, "B4"},   // 11
  {0x15, 0x65, "C5"}    // 12 (fixed: perfect octave above C4)
};

// Pentatonic scale (C D E G A C) - classic 80s sound!
Note pentatonic_scale[] = {
  {0x14, 0x65, "C4"},   // C
  {0x1C, 0x11, "D4"},   // D
  {0x1C, 0x42, "E4"},   // E
  {0x24, 0x17, "G4"},   // G
  {0x24, 0x52, "A4"},   // A
  {0x15, 0x65, "C5"}    // C (perfect octave)
};

// Octaves (C at different octaves)
Note octave_scale[] = {
  {0x0C, 0x32, "C3"},   // Low
  {0x14, 0x65, "C4"},   // Mid (BLOCK=4)
  {0x15, 0x65, "C5"},   // High (BLOCK=5 - perfect octave)
  {0x16, 0x65, "C6"}    // Very high (BLOCK=6 - perfect octave)
};

// Single note (C4 drone)
Note single_note[] = {
  {0x14, 0x65, "C4"}
};

// Scale pointers and lengths
Note* currentScaleNotes = chromatic_scale;
int currentScaleLength = 13;

// ===== SCALE SELECTION =====

const char* scaleNames[] = {
  "Chromatic (12-tone)",
  "Pentatonic (5-note)",
  "Octaves (wide jumps)",
  "Single Note (drone)"
};

void selectScale(int scaleNum) {
  if (scaleNum == currentScale) return;

  currentScale = scaleNum;

  switch(scaleNum) {
    case 0: // Chromatic
      currentScaleNotes = chromatic_scale;
      currentScaleLength = 13;
      break;
    case 1: // Pentatonic
      currentScaleNotes = pentatonic_scale;
      currentScaleLength = 6;
      break;
    case 2: // Octaves
      currentScaleNotes = octave_scale;
      currentScaleLength = 4;
      break;
    case 3: // Single
      currentScaleNotes = single_note;
      currentScaleLength = 1;
      break;
  }

  // Reset arp to start
  arpIndex = 0;
  arpDirection = true;

  Serial.print("♪ Scale: ");
  Serial.println(scaleNames[scaleNum]);
}

// ===== PARAMETER UPDATE =====

void updateParameters() {
  int pot1 = analogRead(POT_RESOLUTION);
  int pot2 = analogRead(POT_RHYTHM);

  // POT 1: Resolution (scale selection)
  if (abs(pot1 - currentResolution) > 50) {
    currentResolution = pot1;
    int newScale = map(pot1, 0, 1023, 0, 3);
    selectScale(newScale);
  }

  // POT 2: Rhythm (tempo)
  if (abs(pot2 - currentRhythm) > 20) {
    currentRhythm = pot2;
    // Map tempo: 400ms (slow) to 50ms (fast)
    currentTempo = map(pot2, 0, 1023, 400, 50);

    int bpm = 60000 / (currentTempo * 4); // Rough BPM estimate
    Serial.print("♫ Tempo: ");
    Serial.print(currentTempo);
    Serial.print("ms (≈");
    Serial.print(bpm);
    Serial.println(" BPM)");
  }
}

// ===== ARPEGGIATOR =====

void playArpeggio() {
  unsigned long now = millis();

  if (now - lastArpTime >= currentTempo) {
    // Play current note
    Note n = currentScaleNotes[arpIndex];
    keyon(n.fnum_h, n.fnum_l);

    // Calculate note duration (shorter than tempo for staccato feel)
    int noteDuration = currentTempo * 0.7;
    delay(noteDuration);
    keyoff();

    // Move to next note
    if (currentScaleLength > 1) {
      if (arpDirection) {
        arpIndex++;
        if (arpIndex >= currentScaleLength) {
          arpIndex = currentScaleLength - 2;
          arpDirection = false;
        }
      } else {
        arpIndex--;
        if (arpIndex < 0) {
          arpIndex = 1;
          arpDirection = true;
        }
      }
    }

    lastArpTime = now;
  }
}

// ===== SETUP =====

void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("   80s SYNTH LEAD - Stranger Things");
  Serial.println("========================================");
  Serial.println("POT 1 (A0): Resolution (scale)");
  Serial.println("  0-25%:   Chromatic (12 notes)");
  Serial.println("  25-50%:  Pentatonic (5 notes)");
  Serial.println("  50-75%:  Octaves (wide)");
  Serial.println("  75-100%: Single note");
  Serial.println("");
  Serial.println("POT 2 (A1): Rhythm (tempo)");
  Serial.println("  Left:  Slow, ambient");
  Serial.println("  Right: Fast, energetic");
  Serial.println("========================================\n");

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(POT_RESOLUTION, INPUT);
  pinMode(POT_RHYTHM, INPUT);
  set_ss_pin(HIGH);

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();

  init_825();
  set_tone(synth_lead_80s);
  set_ch();

  // Initialize with current pot positions
  currentResolution = analogRead(POT_RESOLUTION);
  currentRhythm = analogRead(POT_RHYTHM);

  int initialScale = map(currentResolution, 0, 1023, 0, 3);
  selectScale(initialScale);

  currentTempo = map(currentRhythm, 0, 1023, 400, 50);

  Serial.println("✓ 80s synth ready!");
  Serial.println("Playing arpeggio...\n");
}

// ===== MAIN LOOP =====

void loop() {
  // Check pots
  updateParameters();

  // Play arpeggio
  playArpeggio();

  delay(5);
}
