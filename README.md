# Rayman 3DS Port

A simplified port / emulation engine for the classic **Rayman (PlayStation 1)** adapted for the **Nintendo 3DS** and **New 3DS**, written in **C** using the `libctru` library.

---

## 🚀 Features

- **Unified Controller Mapping:** Support for both the digital **D-Pad** and the **Circle Pad** (Analog) on the 3DS.
- **New 3DS Support:** Automatic detection to enable overclocking (804MHz + L2 Cache) for optimal performance.
- **SD Card Reading:** Direct loading of ISO image files (`sdmc:/Rayman.iso`) via `libfat`.
- **Rendering & Framebuffer:**
  - Color format conversion from **RGB565** (PS1) to **BGR888** (3DS).
  - 90° matrix rotation to match the 3DS hardware RAM display orientation.
  - Automatic centering of the 320x240 PS1 image on the 400x240 3DS top screen.
- **Dual Screen:** Initial support for rendering interface/maps on the bottom screen.
- **Visual Stability:** VBlank synchronization to eliminate screen tearing.

---

## 📂 Project Structure

```text
.
├── source/
│   ├── main.c              # Hardware initialization, main loop, and lifecycle
│   ├── ps1_emulator.c      # Input handling, framebuffer conversion, and engine logic
│   └── ps1_emulator.h      # Header file with function declarations and definitions
└── README.md               # Project documentation
```
 ## 🛠️ Build Requirements
**To compile this project, you need the devkitPro environment installed and configured with:**

•devkitARM

•libctru

•libfat

## 💻 How to Build

1.Ensure devkitPro is installed and environment variables ($DEVKITPRO, $DEVKITARM) are set.

2. Open a terminal in the project root directory and run:
```Bash
make
```
3.The build process will produce a .3dsx executable file (and optionally a .cia file if configured in the Makefile).
## 🎮 How to Play
1.Copy your Rayman game image renamed to Rayman.iso to the root of your SD Card (sdmc:/Rayman.iso).

2.Copy the compiled .3dsx file to /3ds/Rayman/ on your SD Card.

3.Launch the Homebrew Launcher on your Nintendo 3DS and start the application

## **Controls Mapping**

Rayman Action (PS1)	3DS Control

Move (Left / Right)	D-Pad Left/Right or Circle Pad

Look Up / Crouch	D-Pad Up/Down or Circle Pad

Jump (×)	A Button

Fist / Attack (□)	Y Button

Exit Game	START Button
