# YMF825 FM Sound Design Guide

This experiment demonstrates **FM (Frequency Modulation) synthesis** on the YMF825 chip. Upload `fm_sound_design.ino` to hear 8 different preset sounds cycling through a C major scale.

## What You'll Hear

The sketch cycles through these 8 presets:
1. **Electric Piano** - Classic FM e-piano sound
2. **Bright Bell** - Shimmering bell tones
3. **Deep Bass** - Rich, modulated bass
4. **Organ** - Additive organ-like sound
5. **Metallic Clang** - Inharmonic metallic tones
6. **Soft Pad** - Slow-attack ambient pad
7. **Synth Lead** - Bright lead synth
8. **Plucked String** - Fast attack/decay pluck

## FM Synthesis Basics

FM synthesis creates sound by having one oscillator (the **modulator**) control the frequency of another (the **carrier**). The YMF825 has **4 operators** per voice that can be connected in different ways.

### Algorithms (8 types)

**Algorithms** determine how the 4 operators connect:

```
Algorithm 0: 1→2→3→4 (All series - most harmonic complexity)
Algorithm 1: (1→2)→(3→4) (Two pairs in series)
Algorithm 7: 1+2+3+4 (All parallel - additive, organ-like)
```

- **Lower algorithm numbers**: More modulation → more complex harmonics
- **Higher algorithm numbers**: More additive → clearer, organ-like tones

### Envelope (ADSR)

Each operator has an envelope that controls volume over time:

```
    Attack   Decay
       ↗     ↘
      /       \_____ Sustain
     /            \  Release
    /              ↘
```

**Parameters** (each 0-15, higher = faster):
- **AR** (Attack Rate): How fast sound reaches peak
- **DR** (Decay Rate): How fast it drops to sustain
- **SR** (Sustain Rate): How the sustain level changes
- **RR** (Release Rate): How fast it fades after key release

**Examples**:
- Piano: Fast AR, medium DR/SR/RR
- Pad: Slow AR, slow RR
- Pluck: Very fast AR, very fast DR

### Key Parameters in Each Operator

Looking at the tone data format:
```cpp
unsigned char preset[35] = {
  0x81,        // Header
  0x01, 0x85,  // BO, LFO, Algorithm (0x85 = Algorithm 5)

  // Operator 1 (7 bytes):
  0x00,        // SR3-0, XOF, KSR
  0x7F,        // RR3-0, DR3-0
  0xF4,        // AR3-0, SL3-0
  0xBB,        // TL5-0, KSL1-0
  0x00,        // DAM, EAM, DVB, EVB
  0x10,        // MULTI3-0, DT2-0
  0x40,        // WS4-0, FB2-0

  // Operators 2, 3, 4 follow same pattern...
  // ...

  0x80, 0x03, 0x81, 0x80  // End marker
};
```

**Important Parameters**:

- **TL** (Total Level, byte 4): 0-63, **lower = louder**
- **MULTI** (Frequency Multiplier, byte 6):
  - 0 = ×0.5, 1 = ×1, 2 = ×2, etc.
  - Creates harmonic relationships
- **FB** (Feedback, byte 7): 0-7
  - Adds complexity and noise
  - High values create metallic/inharmonic tones
- **WS** (Waveform, byte 7): 0-28
  - 0 = Pure sine wave
  - Higher numbers = more harmonics

## Modifying Sounds

### Easy Changes

**1. Change the algorithm** (byte 2, bits 0-2):
```cpp
0x01, 0x85,  // Change 0x85 to:
             // 0x80 = Algorithm 0 (complex FM)
             // 0x87 = Algorithm 7 (additive)
```

**2. Adjust attack/release** (make it more/less "plucky"):
```cpp
// Byte 3: Higher value = faster attack
0xFF,  // Very fast attack (pluck, bell)
0x3F,  // Slow attack (pad, string)

// Byte 4 upper nibble: Release rate
0xF4,  // Fast release (0xF_)
0x34,  // Slow release (0x3_)
```

**3. Change volume** (byte 4):
```cpp
0xBB,  // Quieter (0xBB = TL high)
0x0E,  // Louder (0x0E = TL low)
```

**4. Adjust harmonics** (byte 6):
```cpp
0x10,  // MULTI = 1 (fundamental)
0x20,  // MULTI = 2 (octave up)
0x30,  // MULTI = 3 (perfect fifth up)
```

**5. Add feedback** (byte 7):
```cpp
0x40,  // No feedback (clean)
0x47,  // Max feedback (metallic)
```

### Example: Make a Warm Bass

```cpp
unsigned char my_bass[35] = {
  0x81,
  0x01, 0x80,  // Algorithm 0 (series modulation)

  // Op1: Slow attack, low freq modulator
  0x00, 0x5F, 0x50, 0xCC, 0x00, 0x01, 0x40,
  //    slow↑  ↑slow    ↑quiet    ↑×0.5

  // Op2: Another modulator
  0x00, 0x6F, 0x60, 0xBB, 0x00, 0x01, 0x40,

  // Op3: Carrier
  0x00, 0x7F, 0x70, 0x9B, 0x00, 0x10, 0x40,
  //                  ↑louder  ↑×1

  // Op4: Final carrier
  0x00, 0x7F, 0x60, 0x0E, 0x00, 0x10, 0x40,
  //                  ↑very loud

  0x80, 0x03, 0x81, 0x80
};
```

### Example: Bright Bell Sound

```cpp
unsigned char my_bell[35] = {
  0x81,
  0x01, 0x84,  // Algorithm 4

  // Op1: Fast attack, high harmonics
  0x00, 0xFF, 0xFF, 0xAA, 0x00, 0x13, 0x45,
  //    fast↑  ↑fast        ↑×3   ↑FB=5

  // Continue with other operators...
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x11, 0x40,
  0x00, 0xFF, 0xFF, 0x8A, 0x00, 0x17, 0x43,
  0x00, 0xFF, 0xF0, 0x0A, 0x00, 0x10, 0x40,

  0x80, 0x03, 0x81, 0x80
};
```

## Experimentation Tips

1. **Start simple**: Modify one parameter at a time
2. **Copy a preset**: Duplicate one that's close to what you want
3. **Listen to differences**: Change algorithm first, then envelopes
4. **Use MULTI creatively**: Try ratios like 3, 5, 7 for bells/chimes
5. **Feedback sparingly**: Usually 0-3, higher for special effects

## Quick Parameter Reference

| Byte | Parameter | Range | Effect |
|------|-----------|-------|--------|
| 2 | Algorithm | 0-7 | Operator routing |
| 3 | SR, XOF, KSR | varies | Sustain behavior |
| 4 (hi) | RR | 0-15 | Release speed |
| 4 (lo) | DR | 0-15 | Decay speed |
| 5 (hi) | AR | 0-15 | Attack speed |
| 5 (lo) | SL | 0-15 | Sustain level |
| 6 (hi) | TL | 0-63 | Volume (lower=louder) |
| 7 (hi) | MULTI | 0-15 | Freq multiplier |
| 7 (lo) | FB | 0-7 | Feedback amount |

## Next Steps

Try creating your own preset by:
1. Copying one of the existing presets
2. Giving it a new name
3. Changing one parameter at a time
4. Adding it to the `presets[]` array
5. Updating `NUM_PRESETS`

Have fun exploring FM synthesis! 🎹
