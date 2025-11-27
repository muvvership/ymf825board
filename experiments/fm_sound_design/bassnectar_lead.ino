/*
  YMF825 Heavy Bass Lead - Bassnectar Style

  A massive, wobbling bass lead with real-time control!
  Perfect for dubstep, bass music, and heavy electronic sounds.

  POT 1 (A0): FM Wobble Intensity (0-100%)
              - Left: Clean, focused bass
              - Right: Heavy modulation, aggressive harmonics

  POT 2 (A1): Detune/Spread (creates thickness)
              - Left: Tight, mono bass
              - Right: Wide, detuned, chorus-like effect

  Hardware:
    YMF825:
      RST_N - Pin 9
      SS    - Pin 10
      MOSI  - Pin 11
      MISO  - Pin 12
      SCK   - Pin 13

    Potentiometers:
      POT 1: GND - A0 - 5V
      POT 2: GND - A1 - 5V

  Pattern: Plays a heavy bass riff that loops
  Twist the pots for MASSIVE sound design! 🔊
*/

#include <SPI.h>

#define OUTPUT_power 0
#define POT_WOBBLE A0    // FM Wobble intensity
#define POT_DETUNE A1    // Detune/spread

// Current parameter values
int currentWobble = -1;
int currentDetune = -1;

// Sequencer
unsigned long lastNoteTime = 0;
int currentStep = 0;

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
   if_s_write(0x00, 0x01); // CLKEN
   if_s_write(0x01, 0x00); // ALRST
   if_s_write(0x1A, 0xA3);
   delay(1);
   if_s_write(0x1A, 0x00);
   delay(30);
   if_s_write(0x02, 0x04); // AP1, AP3
   delay(1);
   if_s_write(0x02, 0x00);
   if_s_write(0x19, 0x3F); // MASTER VOL - MAX
   if_s_write(0x1B, 0x3F); // interpolation
   if_s_write(0x14, 0x00); // interpolation
   if_s_write(0x03, 0x03); // Analog Gain - MAX

   if_s_write(0x08, 0xF6);
   delay(21);
   if_s_write(0x08, 0x00);
   if_s_write(0x09, 0xF8);
   if_s_write(0x0A, 0x00);

   if_s_write(0x17, 0x40); // MS_S
   if_s_write(0x18, 0x00);
}

// ===== BASS LEAD PRESET =====
// Heavy, aggressive bass with lots of modulation potential

unsigned char bass_lead_template[35] = {
  0x81,  // Header
  0x01, 0x82,  // BO=1, LFO=0, Algorithm 2 (good for aggressive FM)

  // Operator 1: Low frequency modulator (sub bass energy)
  0x00,        // SR=0, XOF=0, KSR=0
  0x5F,        // RR=5, DR=15 (medium release, fast decay)
  0xF4,        // AR=15, SL=4 (very fast attack)
  0xCC,        // TL=12 (medium-quiet, will be modulated)
  0x00,        // No amplitude mod
  0x01,        // MULTI=0 (×0.5 - sub bass)
  0x40,        // WS=4, FB=0 (will be modified by pot)

  // Operator 2: Main bass carrier
  0x00,        // SR=0
  0x7F,        // RR=7, DR=15 (fast)
  0xF3,        // AR=15, SL=3 (very fast attack, low sustain)
  0x0E,        // TL=0 (LOUD - main bass)
  0x00,        // No mods
  0x10,        // MULTI=1 (fundamental)
  0x40,        // WS=4, FB=0

  // Operator 3: Harmonic modulator (adds aggression)
  0x00,        // SR=0
  0x8F,        // RR=8, DR=15 (fast envelope)
  0xF5,        // AR=15, SL=5
  0x9B,        // TL=9 (medium)
  0x00,        // No mods
  0x20,        // MULTI=2 (octave up for harmonics)
  0x47,        // WS=4, FB=7 (MAX feedback for aggression!)

  // Operator 4: High harmonic (brightness)
  0x00,        // SR=0
  0x9F,        // RR=9, DR=15
  0xE2,        // AR=14, SL=2
  0xBB,        // TL=11 (quiet, just for color)
  0x00,        // No mods
  0x30,        // MULTI=3 (adds brightness)
  0x42,        // WS=4, FB=2

  0x80, 0x03, 0x81, 0x80  // End marker
};

// Working tone buffer
unsigned char working_tone[35];

// ===== PARAMETER MODIFICATION =====

void applyBassParameters(int wobbleValue, int detuneValue) {
  // Copy template
  memcpy(working_tone, bass_lead_template, 35);

  // POT 1: FM Wobble Intensity (0-1023)
  // Controls feedback and modulator levels for "wobble" effect
  int feedback1 = map(wobbleValue, 0, 1023, 0, 7);  // Op1 feedback
  int feedback3 = map(wobbleValue, 0, 1023, 3, 7);  // Op3 feedback (starts at 3)
  int modDepth = map(wobbleValue, 0, 1023, 0xCC, 0x0E); // Op1 TL (louder = more wobble)

  // Apply wobble to operators
  working_tone[9] = (working_tone[9] & 0xF8) | feedback1;   // Op1 feedback
  working_tone[23] = (working_tone[23] & 0xF8) | feedback3; // Op3 feedback
  working_tone[6] = modDepth; // Op1 total level (modulation depth)

  // POT 2: Detune/Spread (0-1023)
  // Modifies frequency multipliers to create chorusing/detuning
  int detune = map(detuneValue, 0, 1023, 0, 3);

  // Apply detune by shifting algorithms and operator multipliers
  if (detune == 0) {
    // Tight bass - algorithm 2, standard multipliers
    working_tone[2] = (working_tone[2] & 0xF8) | 0x02;
  } else if (detune == 1) {
    // Medium spread - algorithm 3, slight mult changes
    working_tone[2] = (working_tone[2] & 0xF8) | 0x03;
    working_tone[20] = (working_tone[20] & 0xF0) | 0x11; // Op3 multi=1.1×
  } else if (detune == 2) {
    // Wide spread - algorithm 4
    working_tone[2] = (working_tone[2] & 0xF8) | 0x04;
    working_tone[13] = (working_tone[13] & 0xF0) | 0x11; // Op2 detune
    working_tone[27] = (working_tone[27] & 0xF0) | 0x21; // Op4 detune
  } else {
    // MASSIVE spread - algorithm 5, all detuned
    working_tone[2] = (working_tone[2] & 0xF8) | 0x05;
    working_tone[13] = (working_tone[13] & 0xF0) | 0x12; // Op2 multi shift
    working_tone[20] = (working_tone[20] & 0xF0) | 0x21; // Op3 multi shift
    working_tone[27] = (working_tone[27] & 0xF0) | 0x31; // Op4 multi shift
  }
}

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
   if_s_write(0x10, 0x7F); // Channel vol MAX
   if_s_write(0x11, 0x00);
   if_s_write(0x12, 0x08);
   if_s_write(0x13, 0x00);
}

// ===== NOTE CONTROL =====

void keyon(unsigned char fnumh, unsigned char fnuml) {
   if_s_write(0x0B, 0x00);
   if_s_write(0x0C, 0x7F); // Voice vol MAX
   if_s_write(0x0D, fnumh);
   if_s_write(0x0E, fnuml);
   if_s_write(0x0F, 0x40);
}

void keyoff(void) {
   if_s_write(0x0F, 0x00);
}

// ===== BASS NOTE FREQUENCIES =====
// Lower octave notes for heavy bass

struct Note {
  unsigned char fnum_h;
  unsigned char fnum_l;
};

// Low bass notes (around C2-C3 range)
Note note_C2  = {0x0C, 0x32};  // Low C
Note note_D2  = {0x0C, 0x65};  // D
Note note_E2  = {0x0E, 0x21};  // E
Note note_F2  = {0x0E, 0x2E};  // F
Note note_G2  = {0x14, 0x0B};  // G
Note note_A2  = {0x14, 0x29};  // A
Note note_Bb2 = {0x14, 0x38};  // Bb (flat 7th)
Note note_C3  = {0x14, 0x65};  // C (octave up)

// ===== BASS PATTERN =====
// Heavy dubstep-style bass pattern

struct SequenceStep {
  Note note;
  int duration;  // in ms
  bool isRest;
};

// Pattern: C - C - Bb - G (classic bass riff)
SequenceStep bassPattern[] = {
  {note_C2,  400, false},  // C (long)
  {note_C2,  100, true},   // rest
  {note_C2,  200, false},  // C (short)
  {note_C2,  100, true},   // rest
  {note_Bb2, 300, false},  // Bb
  {note_C2,  100, true},   // rest
  {note_G2,  500, false},  // G (long)
  {note_C2,  200, true},   // rest
};

#define PATTERN_LENGTH 8

// ===== PARAMETER UPDATE =====

void updateParameters() {
  int pot1 = analogRead(POT_WOBBLE);
  int pot2 = analogRead(POT_DETUNE);

  // Only update if changed significantly
  if (abs(pot1 - currentWobble) > 10 || abs(pot2 - currentDetune) > 10) {
    currentWobble = pot1;
    currentDetune = pot2;

    applyBassParameters(currentWobble, currentDetune);
    set_tone(working_tone);

    // Print status
    int wobblePct = map(currentWobble, 0, 1023, 0, 100);
    int detunePct = map(currentDetune, 0, 1023, 0, 100);

    Serial.print("🔊 Wobble: ");
    Serial.print(wobblePct);
    Serial.print("%  |  Detune: ");
    Serial.print(detunePct);
    Serial.println("%");
  }
}

// ===== SETUP =====

void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("   HEAVY BASS LEAD - Bassnectar Style");
  Serial.println("========================================");
  Serial.println("POT 1 (A0): FM Wobble (filter sweep)");
  Serial.println("POT 2 (A1): Detune/Spread (thickness)");
  Serial.println("");
  Serial.println("🔊 Playing bass pattern...");
  Serial.println("Twist pots for MASSIVE sounds!");
  Serial.println("========================================\n");

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(POT_WOBBLE, INPUT);
  pinMode(POT_DETUNE, INPUT);
  set_ss_pin(HIGH);

  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();

  init_825();
  set_ch();

  // Initialize with current pot positions
  currentWobble = analogRead(POT_WOBBLE);
  currentDetune = analogRead(POT_DETUNE);
  applyBassParameters(currentWobble, currentDetune);
  set_tone(working_tone);

  Serial.println("✓ Bass synth ready!\n");
}

// ===== MAIN LOOP =====

void loop() {
  // Always check pots for real-time control
  updateParameters();

  // Play bass pattern
  if (millis() - lastNoteTime > 50) { // Check frequently
    SequenceStep step = bassPattern[currentStep];

    if (!step.isRest) {
      // Play note
      keyon(step.note.fnum_h, step.note.fnum_l);
    } else {
      // Rest (silence)
      keyoff();
    }

    // Wait for step duration
    delay(step.duration);

    // Move to next step
    currentStep = (currentStep + 1) % PATTERN_LENGTH;
    lastNoteTime = millis();
  }

  delay(5); // Small delay for stability
}
