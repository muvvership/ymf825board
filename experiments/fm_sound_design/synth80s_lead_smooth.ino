/*
  YMF825 80s Synth Lead - SMOOTH SCALE BLENDING

  Classic 80s synthwave lead with SMOOTH transitions!
  POT 1 now smoothly blends between scales instead of discrete jumps.

  POT 1 (A0): Resolution (SMOOTH blending)
              - 0%:   All 12 chromatic notes
              - 25%:  Smooth transition to pentatonic
              - 50%:  Pure pentatonic (5 notes)
              - 75%:  Transition to octaves
              - 100%: Single note drone

  POT 2 (A1): Rhythm (Tempo)
              - Left:  Slow, ambient
              - Right: Fast, energetic

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

  NOW WITH BUTTER-SMOOTH TRANSITIONS! 🎹✨
*/

#include <SPI.h>

#define OUTPUT_power 0
#define POT_RESOLUTION A0
#define POT_RHYTHM A1

// Current settings
int currentResolution = -1;
int currentRhythm = -1;
int currentTempo = 200;

// Arpeggiator state
int arpIndex = 0;
bool arpDirection = true;
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

unsigned char synth_lead_80s[35] = {
  0x81,
  0x01, 0x87,

  0x00,
  0xFF,
  0xF8,
  0x0E,
  0x00,
  0x10,
  0x40,

  0x00,
  0xFF,
  0xF6,
  0x2E,
  0x00,
  0x20,
  0x48,

  0x00,
  0xFF,
  0xF5,
  0x4E,
  0x00,
  0x30,
  0x4A,

  0x00,
  0xEF,
  0xE4,
  0x5E,
  0x00,
  0x40,
  0x4C,

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
  bool isCore;  // Core notes: C, D, E, G, A (pentatonic)
};

// Master chromatic scale with metadata
Note master_scale[] = {
  {0x14, 0x65, "C4",  true},   // 0  - Core (pentatonic)
  {0x14, 0x73, "C#4", false},  // 1
  {0x1C, 0x11, "D4",  true},   // 2  - Core (pentatonic)
  {0x1C, 0x29, "D#4", false},  // 3
  {0x1C, 0x42, "E4",  true},   // 4  - Core (pentatonic)
  {0x1C, 0x5d, "F4",  false},  // 5
  {0x1C, 0x79, "F#4", false},  // 6
  {0x24, 0x17, "G4",  true},   // 7  - Core (pentatonic)
  {0x24, 0x34, "G#4", false},  // 8
  {0x24, 0x52, "A4",  true},   // 9  - Core (pentatonic)
  {0x24, 0x71, "A#4", false},  // 10
  {0x2C, 0x1d, "B4",  false},  // 11
  {0x15, 0x65, "C5",  true}    // 12 - Core (pentatonic)
};

#define MASTER_SCALE_LENGTH 13

// Active note filtering
bool noteActive[MASTER_SCALE_LENGTH];
int activeNoteCount = 0;
int activeNoteIndices[MASTER_SCALE_LENGTH];

// ===== SMOOTH SCALE BLENDING =====

void updateActiveNotes(int resolution) {
  // resolution: 0-1023 from pot
  // 0-255:   All chromatic notes (100% active)
  // 256-511: Blend to pentatonic (gradually disable non-core notes)
  // 512-767: Blend to octaves (disable all but C notes)
  // 768-1023: Blend to single note (only middle C)

  activeNoteCount = 0;

  if (resolution < 256) {
    // Zone 1: All chromatic notes
    for (int i = 0; i < MASTER_SCALE_LENGTH; i++) {
      noteActive[i] = true;
      activeNoteIndices[activeNoteCount++] = i;
    }
  }
  else if (resolution < 512) {
    // Zone 2: Blend to pentatonic (0-100% blend)
    int blend = map(resolution, 256, 511, 0, 100);

    for (int i = 0; i < MASTER_SCALE_LENGTH; i++) {
      // Core notes always active
      // Non-core notes gradually disabled based on blend
      if (master_scale[i].isCore) {
        noteActive[i] = true;
        activeNoteIndices[activeNoteCount++] = i;
      } else {
        // Probabilistic inclusion that decreases with blend
        noteActive[i] = (random(100) > blend);
        if (noteActive[i]) {
          activeNoteIndices[activeNoteCount++] = i;
        }
      }
    }

    // Ensure at least core notes are active
    if (activeNoteCount == 0) {
      for (int i = 0; i < MASTER_SCALE_LENGTH; i++) {
        if (master_scale[i].isCore) {
          noteActive[i] = true;
          activeNoteIndices[activeNoteCount++] = i;
        }
      }
    }
  }
  else if (resolution < 768) {
    // Zone 3: Blend to octaves (only C notes)
    int blend = map(resolution, 512, 767, 0, 100);

    for (int i = 0; i < MASTER_SCALE_LENGTH; i++) {
      // Only C notes (indices 0 and 12)
      if (i == 0 || i == 12) {
        noteActive[i] = true;
        activeNoteIndices[activeNoteCount++] = i;
      } else if (master_scale[i].isCore) {
        // Pentatonic notes fade out
        noteActive[i] = (random(100) > blend);
        if (noteActive[i]) {
          activeNoteIndices[activeNoteCount++] = i;
        }
      } else {
        noteActive[i] = false;
      }
    }

    // Ensure at least C notes
    if (activeNoteCount < 2) {
      noteActive[0] = true;
      noteActive[12] = true;
      activeNoteCount = 0;
      activeNoteIndices[activeNoteCount++] = 0;
      activeNoteIndices[activeNoteCount++] = 12;
    }
  }
  else {
    // Zone 4: Blend to single note
    int blend = map(resolution, 768, 1023, 0, 100);

    // Only middle C, sometimes high C
    noteActive[0] = true;
    activeNoteIndices[activeNoteCount++] = 0;

    if (random(100) > blend) {
      noteActive[12] = true;
      activeNoteIndices[activeNoteCount++] = 12;
    }
  }
}

// ===== PARAMETER UPDATE =====

void updateParameters() {
  int pot1 = analogRead(POT_RESOLUTION);
  int pot2 = analogRead(POT_RHYTHM);

  // POT 1: Resolution (smooth blending)
  if (abs(pot1 - currentResolution) > 20) {
    currentResolution = pot1;
    updateActiveNotes(currentResolution);

    // Print status
    int pct = map(currentResolution, 0, 1023, 0, 100);
    Serial.print("♪ Resolution: ");
    Serial.print(pct);
    Serial.print("% (");
    Serial.print(activeNoteCount);
    Serial.println(" notes active)");
  }

  // POT 2: Rhythm (tempo)
  if (abs(pot2 - currentRhythm) > 20) {
    currentRhythm = pot2;
    currentTempo = map(pot2, 0, 1023, 400, 50);

    int bpm = 60000 / (currentTempo * 4);
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
    if (activeNoteCount == 0) return; // Safety check

    // Make sure arpIndex is valid
    if (arpIndex >= activeNoteCount) {
      arpIndex = activeNoteCount - 1;
    }
    if (arpIndex < 0) {
      arpIndex = 0;
    }

    // Play current active note
    int noteIdx = activeNoteIndices[arpIndex];
    Note n = master_scale[noteIdx];
    keyon(n.fnum_h, n.fnum_l);

    // Calculate note duration
    int noteDuration = currentTempo * 0.7;
    delay(noteDuration);
    keyoff();

    // Move to next note (bounce pattern)
    if (activeNoteCount > 1) {
      if (arpDirection) {
        arpIndex++;
        if (arpIndex >= activeNoteCount) {
          arpIndex = activeNoteCount - 2;
          if (arpIndex < 0) arpIndex = 0;
          arpDirection = false;
        }
      } else {
        arpIndex--;
        if (arpIndex < 0) {
          arpIndex = 1;
          if (arpIndex >= activeNoteCount) arpIndex = activeNoteCount - 1;
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
  Serial.println("==========================================");
  Serial.println("  80s SYNTH LEAD - SMOOTH SCALE BLENDING");
  Serial.println("==========================================");
  Serial.println("POT 1 (A0): Resolution (SMOOTH!)");
  Serial.println("  0%:   All 12 chromatic notes");
  Serial.println("  50%:  Pentatonic (5 notes)");
  Serial.println("  100%: Single note drone");
  Serial.println("");
  Serial.println("POT 2 (A1): Rhythm (tempo)");
  Serial.println("  Left:  Slow, ambient");
  Serial.println("  Right: Fast, energetic");
  Serial.println("==========================================\n");

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

  // Initialize random seed
  randomSeed(analogRead(A2));

  // Initialize with current pot positions
  currentResolution = analogRead(POT_RESOLUTION);
  currentRhythm = analogRead(POT_RHYTHM);

  updateActiveNotes(currentResolution);
  currentTempo = map(currentRhythm, 0, 1023, 400, 50);

  Serial.println("✓ 80s synth ready!");
  Serial.println("NOW WITH SMOOTH BLENDING!\n");
}

// ===== MAIN LOOP =====

void loop() {
  updateParameters();
  playArpeggio();
  delay(5);
}
