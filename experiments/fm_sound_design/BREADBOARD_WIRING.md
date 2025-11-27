# Breadboard Wiring Guide - FM Synth with Potentiometers

This guide shows how to wire **2 potentiometers** to your Arduino to control FM synthesis parameters in real-time.

## Parts Needed

- Arduino Uno (or compatible)
- YMF825 FM synthesis board
- 2× 10kΩ potentiometers (linear taper, type B)
- Breadboard
- Jumper wires
- Optional: Speaker or headphones with 3.5mm jack

## Potentiometer Basics

A potentiometer (pot) has **3 pins**:

```
        ┌─────┐
        │  ○  │ ← Shaft (turn this!)
        └──┬──┘
     ┌─────┼─────┐
     │     │     │
    Pin1  Pin2  Pin3
    (L)   (M)   (R)

    Pin 1: Left terminal (GND)
    Pin 2: Wiper/Middle (to Arduino analog pin)
    Pin 3: Right terminal (+5V)
```

**How it works**: As you turn the knob, Pin 2 outputs a voltage between 0V and 5V, which the Arduino reads as 0-1023.

## Arduino Pin Connections

### YMF825 Board → Arduino
```
YMF825 Pin    Arduino Pin
──────────    ───────────
RST_N      →  Digital 9
SS         →  Digital 10 (also called CS)
MOSI       →  Digital 11
MISO       →  Digital 12
SCK        →  Digital 13
VCC        →  5V
GND        →  GND
```

### Potentiometers → Arduino
```
POT 1 (Preset Selector):
  Left pin   → GND
  Middle pin → A0
  Right pin  → 5V

POT 2 (FM Intensity):
  Left pin   → GND
  Middle pin → A1
  Right pin  → 5V
```

## Breadboard Layout Diagram

```
                           ARDUINO UNO
                    ┌─────────────────────┐
                    │                     │
                    │   ┌──────────┐      │
                    │   │   USB    │      │
                    │   └──────────┘      │
    ┌───────────────┤                     ├───────────────┐
    │               │  Digital Pins       │               │
    │         ┌─────┤                     ├─────┐         │
    │         │  13 ├─────────────────────┤ 12  │         │
    │         │  11 ├─────────────────────┤ 10  │         │
    │         │   9 ├─────────────────────┤  8  │         │
    │         └─────┤                     ├─────┘         │
    │               │                     │               │
    │         ┌─────┤  Analog Pins        ├─────┐         │
    │         │  A0 ├─────────────────────┤ A1  │         │
    │         └─────┤                     ├─────┘         │
    │               │                     │               │
    │         ┌─────┤  Power              ├─────┐         │
    │         │ GND ├─────────────────────┤ 5V  │         │
    │         └─────┴─────────────────────┴─────┘         │
    └───────────────────────────────────────────────────────┘


  BREADBOARD TOP VIEW:

  Power Rails              Breadboard                Power Rails
  ─────────                                          ─────────
   +  -                                               +  -
   │  │                                               │  │
   ├──┤  1  2  3  4  5    ...    25 26 27 28 29 30  ├──┤
   │  │  │  │  │  │  │           │  │  │  │  │  │   │  │
   Red Blue                                          Red Blue
  (5V)(GND)                                         (5V)(GND)


  Component Placement:

  POT 1 (Preset):          POT 2 (FM Intensity):
      ┌─────┐                  ┌─────┐
      │  ○  │                  │  ○  │
      └──┬──┘                  └──┬──┘
     ┌───┼───┐                ┌───┼───┐
     │   │   │                │   │   │
     │   │   │                │   │   │
     L   M   R                L   M   R
     │   │   │                │   │   │
    GND  A0  5V              GND  A1  5V
```

## Step-by-Step Wiring Instructions

### Part 1: YMF825 Board to Arduino

1. **Connect SPI pins** (YMF825 → Arduino):
   - YMF825 **SS** → Arduino **Pin 10**
   - YMF825 **MOSI** → Arduino **Pin 11**
   - YMF825 **MISO** → Arduino **Pin 12**
   - YMF825 **SCK** → Arduino **Pin 13**
   - YMF825 **RST_N** → Arduino **Pin 9**

2. **Connect power**:
   - YMF825 **VCC** → Arduino **5V**
   - YMF825 **GND** → Arduino **GND**

### Part 2: Power Rails on Breadboard

3. **Set up power rails**:
   - Run a jumper from Arduino **5V** → Breadboard **+ rail** (red)
   - Run a jumper from Arduino **GND** → Breadboard **- rail** (blue/black)

### Part 3: Potentiometer 1 (Preset Selector)

4. **Insert POT 1** into breadboard (use rows 5-7 or similar):
   - Place pot so all 3 pins are in different rows
   - Face the shaft towards you

5. **Wire POT 1**:
   - **Left pin** (Pin 1) → Breadboard **GND rail** (-)
   - **Middle pin** (Pin 2/wiper) → Arduino **A0**
   - **Right pin** (Pin 3) → Breadboard **+5V rail** (+)

### Part 4: Potentiometer 2 (FM Intensity)

6. **Insert POT 2** into breadboard (use rows 15-17 or similar):
   - Leave some space from POT 1
   - Face the shaft towards you

7. **Wire POT 2**:
   - **Left pin** (Pin 1) → Breadboard **GND rail** (-)
   - **Middle pin** (Pin 2/wiper) → Arduino **A1**
   - **Right pin** (Pin 3) → Breadboard **+5V rail** (+)

### Part 5: Output (Optional)

8. **Connect audio output** from YMF825:
   - Use the YMF825's built-in speaker output, OR
   - Connect to headphones/external speaker via 3.5mm jack on board

## Visual Wiring Schematic

```
         ┌──────────────────┐
         │   ARDUINO UNO    │
         │                  │
         │  Pin 13 ────────────── SCK  ─┐
         │  Pin 12 ────────────── MISO  │
         │  Pin 11 ────────────── MOSI  │  YMF825
         │  Pin 10 ────────────── SS    │  BOARD
         │  Pin 9  ────────────── RST_N │
         │                              │
         │  5V ─────────┬───────────── VCC  │
         │              │              GND ─┘
         │              │               │
         │              │               └──┐
         │  GND ────────┼──────────────────┤
         │              │                  │
         │              │  POT 1           │  POT 2
         │              │  ┌─────┐         │  ┌─────┐
         │              │  │  ○  │         │  │  ○  │
         │              │  └──┬──┘         │  └──┬──┘
         │              │     │            │     │
         │              │  ┌──┼──┐         │  ┌──┼──┐
         │              ├──┤  │  ├─────────┼──┤  │  │
         │              │  │  │  │         │  │  │  │
         │  A0 ────────────┼──┘  │         │  │  └──┼─── A1
         │                 │     │         │  │     │
         └─────────────────┼─────┼─────────┼──┼─────┘
                          GND   5V        GND 5V

    Legend:
    ────  Wire
    ─┐
     │    Multiple connections
     │
    ─┴─   Ground
```

## Testing the Connections

### Before powering on:

1. **Double-check all connections** with the diagrams above
2. **Verify pot orientation**: Middle pin to A0/A1, outer pins to GND and 5V
3. **Check no short circuits**: GND and 5V should never connect directly

### After uploading code:

1. **Open Serial Monitor** (9600 baud)
2. **Turn POT 1** slowly - you should see preset names change
3. **Turn POT 2** slowly - you should see "FM Intensity" percentage change
4. **Listen** as the sound changes in real-time!

## Troubleshooting

### Pot doesn't seem to work:
- Check that middle pin goes to correct analog pin (A0 or A1)
- Verify outer pins connect to GND and 5V
- Try swapping the outer pins (this just reverses the pot direction)

### Sound is distorted:
- Lower the master volume in code
- Check YMF825 power supply is stable
- Reduce analog gain setting

### No sound:
- Verify all YMF825 SPI connections
- Check speaker/headphone connection
- Open serial monitor to see if code is running

### Erratic behavior:
- Pots may need cleaning (spray contact cleaner)
- Add a small capacitor (0.1µF) between A0/A1 and GND
- Check for loose breadboard connections

## What the Pots Control

### POT 1 (A0) - Preset Selector
Chooses one of 8 preset sounds:
- **0**: Electric Piano
- **1**: Bright Bell
- **2**: Deep Bass
- **3**: Organ
- **4**: Metallic Clang
- **5**: Soft Pad
- **6**: Synth Lead
- **7**: Plucked String

### POT 2 (A1) - FM Intensity
Dynamically modifies:
- **Feedback amount** (0-7): More feedback = more harmonics/noise
- **Algorithm shift** (+0 to +3): Changes operator routing slightly

Turn POT 2 from left to right to hear the sound morph from clean to complex!

## Next Steps

Once you have this working, you could:
- Add more pots for attack, release, volume, etc.
- Add buttons to trigger notes manually
- Connect a MIDI input
- Add LEDs to show current preset
- Create a sequencer with rhythm patterns

Happy experimenting! 🎛️🎵
