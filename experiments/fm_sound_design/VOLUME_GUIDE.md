# Volume Troubleshooting Guide

## Quick Fix - Can't Hear Anything?

The YMF825 has **4 volume controls**. Here's how to make it louder:

### 1. CHECK YOUR AUDIO OUTPUT FIRST! 🔊

**Does your YMF825 board have:**
- Built-in speaker? (Look for a small speaker on the board)
- 3.5mm headphone jack? (Connect headphones or external speaker)
- Screw terminals? (Connect speaker wires)

**If using headphones/external speaker:**
- Make sure they're plugged in firmly
- Try turning up the external volume
- Test with different headphones/speakers

---

## Volume Settings in Code

The YMF825 has 4 volume parameters you can adjust:

### Volume Parameter Table

| Parameter | Register | Current | Max | Location in Code |
|-----------|----------|---------|-----|------------------|
| **Analog Gain** | 0x03 | 0x01 | 0x03 | Line ~100 in init_825() |
| **Master Volume** | 0x19 | 0x20 | 0x3F | Line ~97 in init_825() |
| **Channel Volume** | 0x10 | 0x71 | 0x7F | Line ~256 in set_ch() |
| **Voice Volume** | 0x0C | 0x54 | 0x7F | Line ~266 in keyon() |

### ⚠️ MOST COMMON ISSUE: Analog Gain Too Low

Line ~100 says:
```cpp
if_s_write(0x03, 0x01); // Analog Gain - TOO QUIET!
```

**Change it to:**
```cpp
if_s_write(0x03, 0x03); // Analog Gain - MAXIMUM!
```

This is usually the main problem!

---

## How to Change Volume

### Option 1: Quick Fix (Change Analog Gain)

Find this line in `init_825()` function:
```cpp
if_s_write(0x03, 0x01); // Analog Gain
```

**Change to:**
```cpp
if_s_write(0x03, 0x03); // Analog Gain - MAX
```

### Option 2: Increase Master Volume

Find this line in `init_825()`:
```cpp
if_s_write(0x19, 0x20); // MASTER VOL
```

**Change to:**
```cpp
if_s_write(0x19, 0x3F); // MASTER VOL - MAX (63)
```

### Option 3: Increase Channel Volume

Find this line in `set_ch()`:
```cpp
if_s_write(0x10, 0x71); // chvol
```

**Change to:**
```cpp
if_s_write(0x10, 0x7F); // chvol - MAX (127)
```

### Option 4: Increase Voice Volume

Find this line in `keyon()`:
```cpp
if_s_write(0x0C, 0x54); // vovol
```

**Change to:**
```cpp
if_s_write(0x0C, 0x7F); // vovol - MAX (127)
```

---

## Recommended Settings for Loud Output

```cpp
// In init_825() function:
if_s_write(0x03, 0x03); // Analog Gain - MAX
if_s_write(0x19, 0x3F); // Master Volume - MAX

// In set_ch() function:
if_s_write(0x10, 0x7F); // Channel Volume - MAX

// In keyon() function:
if_s_write(0x0C, 0x7F); // Voice Volume - MAX
```

---

## Volume Hierarchy

The volumes multiply together, so if ANY of them is too low, you won't hear much:

```
Final Volume = Analog Gain × Master Vol × Channel Vol × Voice Vol
```

**Start with:**
1. Set Analog Gain to MAX (0x03) first
2. Then adjust Master Volume (0x20 to 0x3F)
3. Fine-tune with Channel/Voice volumes

---

## Still No Sound?

### Hardware Checks:
- ✓ Is the YMF825 powered? (Check LED if it has one)
- ✓ Are all SPI wires connected correctly?
- ✓ Is RST_N (Pin 9) connected?
- ✓ Is speaker/headphones connected?
- ✓ Try different audio output (if available)

### Software Checks:
- ✓ Open Serial Monitor - do you see preset changes?
- ✓ Does LED on Arduino blink when uploading?
- ✓ Re-upload the sketch
- ✓ Try the original sample1 sketch to verify hardware works

### Quick Test:
Upload the original `/home/user/ymf825board/sample1/ymf825board_sample1/ymf825board_sample1.ino` to verify your hardware works. If that plays sound, then it's just a volume setting issue!

---

## Sound is Distorted?

If you turned the volume **too high**:
- Reduce Master Volume to 0x30
- Reduce Analog Gain to 0x02
- Lower the tone TL values (total level) in presets

---

## Next: I'll create a "loud" version for you! 📢
