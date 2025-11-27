# 80s Synth Lead - Resolution & Rhythm Control 🎹✨

A classic 80s synthwave lead with **arpeggiator** and **real-time scale/tempo control**. Perfect for Stranger Things vibes, retro gaming, and synthwave music!

## What It Does

This sketch creates a **bright 80s-style synthesizer lead** that plays **ascending/descending arpeggios**. Control the **note density** and **rhythm** in real-time with 2 potentiometers!

### The Sound

- **Bright, cutting tone** - classic 80s character
- **Additive synthesis** (Algorithm 7 - all operators in parallel)
- **Fast attack** for punchy, articulate notes
- **Harmonic richness** with multiple octaves
- **Arpeggiator** plays up and down patterns

## Potentiometer Controls

### 🎛️ POT 1 (A0): Resolution (Note Density/Scale)

Controls which **notes/scale** the arpeggiator plays. Changes the musical complexity!

**Position Guide:**

| Position | Scale | Notes | Character |
|----------|-------|-------|-----------|
| **0-25%** | Chromatic | 12 notes (all semitones) | Complex, jazzy, tense |
| **25-50%** | Pentatonic | 5 notes (C-D-E-G-A) | Classic 80s, always sounds good! |
| **50-75%** | Octaves | 4 notes (C3-C4-C5-C6) | Wide, dramatic jumps |
| **75-100%** | Single Note | 1 note (C4 drone) | Pulsing, rhythmic texture |

**Musical Uses:**

**Chromatic (0-25%):**
- Most dense/complex
- All 12 chromatic notes (C, C#, D, D#, E, F, F#, G, G#, A, A#, B, C)
- Great for: Tension, complexity, jazz-fusion vibes
- Sounds like: Sci-fi sequences, complex arpeggios

**Pentatonic (25-50%):** ⭐ **MOST MUSICAL**
- 5 notes: C, D, E, G, A, C
- The "can't go wrong" scale
- Classic 80s synthwave sound
- Great for: Melodies that always sound good together
- Sounds like: Stranger Things, synthwave, uplifting sequences

**Octaves (50-75%):**
- Wide jumps between C notes at different octaves
- Dramatic and spacious
- Great for: Build-ups, drops, epic moments
- Sounds like: Stadium synth, wide sweeps

**Single Note (75-100%):**
- Just repeats C4
- Creates a rhythmic pulse/texture
- Great for: Minimal techno, drone, rhythm emphasis
- Sounds like: Pulsing synth pad, metronome-like

---

### 🎛️ POT 2 (A1): Rhythm (Tempo)

Controls the **speed** of the arpeggio - from slow ambient to fast techno!

**Position Guide:**

| Position | Tempo | Note Length | BPM (approx) | Vibe |
|----------|-------|-------------|--------------|------|
| **Left (0%)** | 400ms | Long | ~38 BPM | Slow, ambient, dreamy |
| **25%** | 300ms | Medium-long | ~50 BPM | Relaxed, chill |
| **50%** | 200ms | Medium | ~75 BPM | Classic synthwave |
| **75%** | 100ms | Short | ~150 BPM | Energetic, upbeat |
| **Right (100%)** | 50ms | Very short | ~300 BPM | Fast, techno, frantic |

**Musical Uses:**

**Slow (Left):**
- Ambient, atmospheric
- Each note clearly heard
- Great for: Pads, textures, background
- Sounds like: Blade Runner, ambient synth

**Medium (Center):**
- Classic synthwave tempo
- Good groove and movement
- Great for: Main melodies, hooks
- Sounds like: Stranger Things theme

**Fast (Right):**
- High energy, dancing
- Creates texture through speed
- Great for: Build-ups, climaxes, techno
- Sounds like: Kraftwerk, euro-techno

---

## The Arpeggiator

The arpeggiator plays notes **up and down** in a bouncing pattern:

```
C → D → E → G → A → C5 → A → G → E → D → C → (repeat)
```

**Pattern:**
1. Plays notes ascending (up the scale)
2. When it reaches the top, reverses direction
3. Plays notes descending (down the scale)
4. When it reaches the bottom, reverses again
5. Continues bouncing forever!

**Note Duration:**
- Each note plays for 70% of the tempo
- 30% is silence (creates staccato, punchy feel)
- Makes it sound articulate and clear

## How to Use

### Upload and Play

1. **Wire up the pots** (same as before - see BREADBOARD_WIRING.md)
2. **Upload** `synth80s_lead.ino`
3. **Open Serial Monitor** (9600 baud) to see scale/tempo changes
4. **Listen!** Arpeggio starts immediately
5. **Twist the pots** and hear the music change!

### Sound Design Tips

**Classic Synthwave Sound:**
- POT 1: 25-50% (Pentatonic scale)
- POT 2: 40-60% (Medium tempo, ~100-150ms)
- Result: Classic 80s arpeggio like Stranger Things!

**Ambient Atmosphere:**
- POT 1: 50-75% (Octaves - wide jumps)
- POT 2: 0-25% (Slow tempo)
- Result: Spacious, cinematic pads

**High Energy Techno:**
- POT 1: 0-25% (Chromatic - all notes)
- POT 2: 75-100% (Fast tempo)
- Result: Frantic, busy arpeggios

**Minimal Pulse:**
- POT 1: 75-100% (Single note)
- POT 2: 50% (Medium tempo)
- Result: Pulsing rhythm, minimal techno

**Build-Up Effect:**
1. Start: POT 1 at right (single note), POT 2 at left (slow)
2. Slowly turn POT 1 left (more notes)
3. Slowly turn POT 2 right (faster)
4. Result: Builds from simple pulse to complex fast arpeggio!

## Technical Details

### FM Synthesis Configuration

**Algorithm 7** (All operators in parallel - additive synthesis)
- Bright, organ-like, rich harmonics
- Each operator adds a different frequency component

**Operators:**
1. **Op1:** Fundamental (×1) - TL=0 (LOUD)
2. **Op2:** Octave up (×2) - TL=2 (bright)
3. **Op3:** Fifth (×3) - TL=4 (harmonic richness)
4. **Op4:** Two octaves (×4) - TL=5 (sizzle/air)

**Envelope:**
- **AR=15** (very fast attack) - instant response
- **DR=15** (fast decay) - punchy character
- **SL=8** (high sustain) - maintains brightness
- **RR=15** (fast release) - clean note separation

### Scales Used

**Chromatic (13 notes):**
```
C4  C# D  D# E  F  F# G  G# A  A# B  C5
 1  2  3  4  5  6  7  8  9  10 11 12 13
```

**Pentatonic (6 notes):**
```
C4  D  E  G  A  C5
 1  2  3  4  5  6
```
*The "black keys" scale - always sounds musical!*

**Octaves (4 notes):**
```
C3   C4   C5   C6
Low  Mid  High VeryHigh
```

**Single (1 note):**
```
C4 (repeating)
```

### Tempo Range

- **Slowest:** 400ms per note (~38 BPM)
- **Fastest:** 50ms per note (~300 BPM)
- **Sweet spot:** 150-200ms (~90-120 BPM)

## Modifying the Code

### Change the Base Note

Want to play in a different key? Change the scales:

```cpp
// For A minor instead of C major:
Note pentatonic_scale[] = {
  {0x24, 0x52, "A4"},   // A (instead of C)
  {0x2C, 0x1d, "B4"},   // B (instead of D)
  {0x2C, 0x65, "C5"},   // C (instead of E)
  {0x2C, 0x79, "D5"},   // D (instead of G)
  {0x34, 0x42, "E5"},   // E (instead of A)
  {0x34, 0x52, "A5"}    // A (octave)
};
```

### Change Arpeggiator Pattern

Want to only go up, not down?

```cpp
void playArpeggio() {
  // ... existing code ...

  // Only ascending (comment out the down direction):
  if (arpDirection) {
    arpIndex++;
    if (arpIndex >= currentScaleLength) {
      arpIndex = 0;  // Loop back to start
      // arpDirection = false;  // Comment this out
    }
  }
}
```

### Add Random Notes

Want random note selection?

```cpp
void playArpeggio() {
  unsigned long now = millis();

  if (now - lastArpTime >= currentTempo) {
    // Play random note instead of sequential
    int randomIndex = random(0, currentScaleLength);
    Note n = currentScaleNotes[randomIndex];
    keyon(n.fnum_h, n.fnum_l);

    // ... rest of code ...
  }
}
```

### Change Note Duration

Want longer, more legato notes?

```cpp
int noteDuration = currentTempo * 0.9;  // Was 0.7, now 90% (more legato)
```

Want shorter, more staccato notes?

```cpp
int noteDuration = currentTempo * 0.5;  // 50% (very staccato)
```

## Performance Ideas

### Live Tweaking

**Intro:**
- Start with single note (POT 1 right), slow tempo (POT 2 left)
- Creates minimal, pulsing intro

**Build:**
- Slowly move POT 1 left (adds more notes)
- Slowly move POT 2 right (speeds up)

**Drop:**
- Slam POT 1 to 25% (pentatonic - sweet spot)
- Set POT 2 to 60% (energetic tempo)

**Break:**
- Move POT 1 to 50-75% (octaves - sparse)
- Slow down POT 2 to 30%

**Finale:**
- POT 1 to 0% (chromatic - maximum complexity)
- POT 2 to 80-90% (fast and furious)

### Layering

The YMF825 has 16 voices! You could:
- Run this sketch for arpeggio
- Add bass from `bassnectar_lead.ino`
- Create full arrangements!

## Troubleshooting

### Arpeggio too fast/slow even at pot extremes

Adjust tempo range:
```cpp
currentTempo = map(pot2, 0, 1023, 600, 80);  // Slower to faster
```

### Want different scales

Add your own! For blues scale:
```cpp
Note blues_scale[] = {
  {0x14, 0x65, "C4"},   // C
  {0x1C, 0x29, "Eb4"},  // Eb (flat 3rd)
  {0x1C, 0x5d, "F4"},   // F
  {0x1C, 0x79, "F#4"},  // F# (blue note)
  {0x24, 0x17, "G4"},   // G
  {0x2C, 0x1d, "Bb4"},  // Bb (flat 7th)
  {0x2C, 0x65, "C5"}    // C
};
```

### Sound too bright/harsh

Lower operator volumes (TL values):
```cpp
0x2E,  // Was 0x0E, now quieter
```

Or reduce analog gain:
```cpp
if_s_write(0x03, 0x02);  // Was 0x03, now medium
```

## What Makes This "80s"?

Classic 80s synth characteristics:
- ✅ Bright, cutting tone (additive synthesis)
- ✅ Fast attack (instant response)
- ✅ Arpeggiated patterns (sequencer-style)
- ✅ Pentatonic scales (always musical)
- ✅ Staccato articulation (punchy notes)
- ✅ Clean, precise timing

This captures the essence of 80s synthesizers like:
- Roland Juno-106
- Yamaha DX7
- Sequential Circuits Prophet-5

## Next Steps

Try adding:
- More voices (chords, layers)
- MIDI input for note control
- Buttons to change scales
- LFO for vibrato
- Different waveforms

Get ready for some **SYNTHWAVE**! 🎹✨🌆
