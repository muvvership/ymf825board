# Heavy Bass Lead - Bassnectar Style 🔊

A massive, wobbling bass synthesizer inspired by heavy bass music and dubstep. Perfect for experimenting with aggressive FM synthesis and dynamic sound design!

## What It Does

This sketch creates a **heavy bass lead** sound and plays a repeating bass riff. You control the sound in real-time with **2 potentiometers** for expressive, dynamic performance.

### The Sound

- **Heavy low-end** bass frequencies (C2-C3 range)
- **Aggressive FM modulation** with high feedback
- **Wobble effect** similar to dubstep bass
- **Detuning/chorusing** for thick, wide sound
- **Fast attack** for punchy, aggressive character

## Potentiometer Controls

### 🎛️ POT 1 (A0): FM Wobble Intensity

Controls the **modulation depth** and **feedback** to create a "wobble" or filter sweep effect.

**Left (0%):**
- Clean, focused bass
- Minimal modulation
- Tight, punchy sound

**Middle (50%):**
- Moderate wobble
- Dynamic harmonics
- Expressive movement

**Right (100%):**
- MAXIMUM wobble
- Heavy modulation
- Aggressive, screaming harmonics
- Similar to dubstep "wub wub" bass

**What it does internally:**
- Adjusts FM feedback (0-7)
- Controls modulator volume (TL)
- Creates filter sweep-like effect

---

### 🎛️ POT 2 (A1): Detune/Spread

Controls **detuning** and **chorus effect** to make the bass wider and thicker.

**Left (0%):**
- Tight, mono bass
- Focused low-end
- Punchy and direct

**25%:**
- Slight spread
- Algorithm shift to 3
- Subtle chorusing

**50%:**
- Medium width
- Algorithm 4
- Noticeable stereo effect

**Right (100%):**
- MASSIVE spread
- Algorithm 5
- All operators detuned
- Wide, evolving, chorus-like sound

**What it does internally:**
- Shifts between algorithms 2-5
- Modifies operator frequency multipliers
- Creates detuning and phase differences

## The Bass Pattern

The sketch plays this repeating bass riff:

```
C  (long)  - 400ms
-- (rest)  - 100ms
C  (short) - 200ms
-- (rest)  - 100ms
Bb (med)   - 300ms
-- (rest)  - 100ms
G  (long)  - 500ms
-- (rest)  - 200ms

(Then loops)
```

This is a classic heavy bass pattern you'd hear in dubstep or bass music!

## How to Use

### Upload and Play

1. **Wire up the pots** (see BREADBOARD_WIRING.md)
2. **Upload** `bassnectar_lead.ino`
3. **Open Serial Monitor** (9600 baud)
4. **Listen!** Bass pattern starts playing immediately

### Sound Design Tips

**For Classic Dubstep Wobble:**
- POT 1: Sweep from left to right slowly
- POT 2: Keep at 25-50%
- Creates the signature "wub wub wub" sound

**For Massive Detuned Bass:**
- POT 1: Medium (50%)
- POT 2: Maximum right
- Wide, evolving, chorus-like sound

**For Tight, Punchy Bass:**
- POT 1: Left (0-25%)
- POT 2: Left (0%)
- Focused, mono sub bass

**For Aggressive Lead:**
- POT 1: Maximum right
- POT 2: 50-75%
- Screaming, aggressive, metallic

## Technical Details

### FM Synthesis Configuration

**Algorithm:** Varies 2-5 based on POT 2
- **Alg 2:** Good aggressive FM routing
- **Alg 3-5:** More complex, detuned, wider

**Operators:**
1. **Op1:** Sub bass modulator (×0.5 frequency)
2. **Op2:** Main bass carrier (×1, fundamental)
3. **Op3:** Harmonic modulator (×2, octave up, high feedback)
4. **Op4:** Brightness (×3, adds sizzle)

**Envelope:** Fast attack (AR=15), medium decay, fast release
- Creates punchy, percussive bass
- Good for rhythmic patterns

**Feedback:** 0-7, controlled by POT 1
- Higher feedback = more harmonics and aggression

### Frequency Range

Uses **low bass notes** (C2-C3, ~65-130 Hz):
- C2:  ~65 Hz
- G2:  ~98 Hz
- Bb2: ~117 Hz
- C3:  ~130 Hz

Perfect for sub bass and heavy low-end!

## Modifying the Pattern

Want a different bass line? Edit the `bassPattern[]` array:

```cpp
SequenceStep bassPattern[] = {
  {note_C2,  400, false},  // Play C2 for 400ms
  {note_C2,  100, true},   // Rest for 100ms
  {note_E2,  300, false},  // Play E2 for 300ms
  // Add more steps!
};

#define PATTERN_LENGTH 3  // Update this to match!
```

### Available Notes

```cpp
note_C2   // Lowest
note_D2
note_E2
note_F2
note_G2
note_A2
note_Bb2
note_C3   // Highest (octave up)
```

### Step Format

```cpp
{note, duration_ms, is_rest}
```

- **note:** Which note to play
- **duration_ms:** How long (milliseconds)
- **is_rest:** `true` = silence, `false` = play note

## Performance Ideas

### Manual Wobble
Slowly turn POT 1 back and forth in time with the bass pattern for manual wobble bass effect!

### Thickness Sweep
Start with POT 2 at left, slowly move to right over 8 bars to build intensity.

### Dubstep Drop
- Start: Both pots at left (tight, clean)
- Build: Slowly increase POT 1
- DROP: Slam both pots to maximum!

### Sub Bass
- POT 1: 0%
- POT 2: 0%
- Pure, clean sub bass for low-end foundation

## Adding More Voices

The YMF825 has **16 voices**! You could modify the code to:
- Play chords (multiple notes at once)
- Add a high lead on top of bass
- Create call-and-response patterns
- Layer multiple bass sounds

## Troubleshooting

### Too Much Distortion
- Lower Master Volume (line ~86): `if_s_write(0x19, 0x30);`
- Reduce Analog Gain (line ~89): `if_s_write(0x03, 0x02);`

### Want More Wobble
- Increase feedback range in `applyBassParameters()`
- Change: `map(wobbleValue, 0, 1023, 3, 7)` → `map(wobbleValue, 0, 1023, 5, 7)`

### Bass Too Low/High
- Modify note frequencies in the Note definitions
- Or transpose the pattern by using different notes

### Pattern Too Fast/Slow
- Change duration values in `bassPattern[]`
- Make all durations 2× longer for slower groove

## What Makes This "Bassnectar-ish"?

Bassnectar's signature sound includes:
- ✅ Heavy sub bass frequencies
- ✅ Dynamic wobble/modulation
- ✅ Aggressive, metallic harmonics
- ✅ Wide, detuned sounds
- ✅ Rhythmic, syncopated patterns

This sketch captures these elements using FM synthesis!

## Next Steps

Try combining this with:
- MIDI input for live performance
- More pots for attack/release control
- Buttons to change patterns
- LFO for automatic wobble
- Multiple bass layers

Rock out and make some HEAVY BASS! 🔊🎵💥
