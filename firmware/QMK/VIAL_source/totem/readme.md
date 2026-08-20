# TOTEM 20

Unibody **20-key macropad** with a rotary encoder and an SSD1306 OLED, built on
a Waveshare RP2040-Zero.

Forked from [TOTEM](https://github.com/GEIGEIGEIST/TOTEM), the 38-key
column-staggered split by @geigeigeist, and rebuilt as a one-piece board. It is
**not split**: no second half, no serial link, no handedness, no `EE_HANDS`.

## Hardware

| | |
|---|---|
| MCU | Waveshare RP2040-Zero |
| Matrix | 4 rows x 6 cols — 20 keys + encoder button = 21 switches |
| Diodes | one per switch, `COL2ROW` |
| RGB | 20 addressable LEDs, one per key, data on `GP26`, 5 V from VBUS |
| Encoder | EC11, `GP14`/`GP15`; its push switch is a matrix key |
| Display | SSD1306 128x64 I2C, `GP10` (SDA) / `GP11` (SCL) |

### Why not the XIAO RP2040

The original TOTEM used a Seeed XIAO RP2040, which exposes **11 GPIO**. The
feature set needs **15**:

```
6 cols + 4 rows + 1 RGB + 2 encoder + 2 I2C = 15
```

No arrangement fits: even encoder-only with no OLED and no RGB needs 12. The
matrix cannot be shrunk either — 21 switches need at least 10 pins (4x6, 5x5
and 3x7 all cost 10). The RP2040-Zero exposes 20 GPIO on its castellated edge,
so all 15 fit with 5 to spare.

The **encoder push switch costs no GPIO**: it is wired into the matrix at
`[0,5]`, one of the positions the 4x6 grid leaves empty.

### Pinout

| Signal | GPIO | Notes |
|---|---|---|
| COL1..COL6 | `GP0` `GP1` `GP2` `GP3` `GP4` `GP5` | |
| ROW1..ROW4 | `GP6` `GP7` `GP8` `GP9` | |
| OLED SDA | `GP10` | I2C1 |
| OLED SCL | `GP11` | I2C1 |
| Encoder A | `GP14` | |
| Encoder B | `GP15` | |
| RGB data | `GP26` | WS2812 chain |
| free | `GP12` `GP13` `GP27` `GP28` `GP29` | |

`GP16` is deliberately unused — it drives the RP2040-Zero's onboard WS2812.

**The I2C pins are not arbitrary.** The RP2040 muxes I2C onto fixed pin groups;
`GP10`/`GP11` is a valid I2C1 SDA/SCL pair. I2C1 specifically, because ChibiOS
ships with `RP_I2C_USE_I2C1 TRUE` and `RP_I2C_USE_I2C0 FALSE` — using I2C0
would additionally need a custom `mcuconf.h`. The `halconf.h` in this folder
turns on `HAL_USE_I2C`.

### Matrix index convention

The firmware indexes rows top-to-bottom, the **reverse** of the schematic row
names. Keep this handy when probing:

| Firmware | Schematic | GPIO |
|---|---|---|
| row 0 | ROW4 | `GP9` |
| row 1 | ROW3 | `GP8` |
| row 2 | ROW2 | `GP7` |
| row 3 | ROW1 | `GP6` |
| col 0..5 | COL1..COL6 | `GP0`..`GP5` |

### Switch map

| SW | Row | Col | `LAYOUT` pos | Game keycode |
|---|---|---|---|---|
| SW1  | 0 (ROW4) | 1 (COL2) | 1  | `KC_1` |
| SW2  | 0 (ROW4) | 2 (COL3) | 2  | `KC_2` |
| SW3  | 0 (ROW4) | 3 (COL4) | 3  | `KC_3` |
| SW4  | 0 (ROW4) | 4 (COL5) | 4  | `KC_R` |
| **ENC** | 0 (ROW4) | 5 (COL6) | 5 | encoder button |
| SW5  | 1 (ROW3) | 0 (COL1) | 6  | `KC_Q` |
| SW6  | 1 (ROW3) | 1 (COL2) | 7  | `KC_A` |
| SW7  | 1 (ROW3) | 2 (COL3) | 8  | `KC_W` |
| SW8  | 1 (ROW3) | 3 (COL4) | 9  | `KC_D` |
| SW9  | 1 (ROW3) | 4 (COL5) | 10 | `KC_F` |
| SW10 | 2 (ROW2) | 0 (COL1) | 11 | `KC_LSFT` |
| SW11 | 2 (ROW2) | 1 (COL2) | 12 | `KC_Z` |
| SW12 | 2 (ROW2) | 2 (COL3) | 13 | `KC_S` |
| SW13 | 2 (ROW2) | 3 (COL4) | 14 | `KC_X` |
| SW14 | 2 (ROW2) | 4 (COL5) | 15 | `KC_T` |
| SW15 | 2 (ROW2) | 5 (COL6) | 16 | `KC_E` |
| SW16 | 3 (ROW1) | 0 (COL1) | 17 | `KC_G` |
| SW17 | 3 (ROW1) | 1 (COL2) | 18 | `KC_LCTL` |
| SW18 | 3 (ROW1) | 3 (COL4) | 19 | `KC_C` |
| SW19 | 3 (ROW1) | 4 (COL5) | 20 | `KC_SPC` |
| SW20 | 3 (ROW1) | 5 (COL6) | 21 | `KC_B` |

Positions `[0,0]`, `[1,5]` and `[3,2]` have no switch and are `KC_NO`.

## Profiles

Five Vial-configurable layers, all 20 keys freely assignable in each:

| # | Profile | Indicator |
|---|---|---|
| 0 | Jogo | red |
| 1 | 3ds Max | blue |
| 2 | Macros / uso geral | green |
| 3 | Reservada | purple |
| 4 | Configuracao / manutencao | yellow |

### Switching profiles

Three ways, all landing on the same place:

- **Encoder short click** — next profile.
- **G + B held together for 1 s** — the original chord, kept because it works
  with the encoder unplugged.
- **OLED menu → Perfil** — the only route that reaches layers 3 and 4, which
  the other two deliberately skip.

Both cycles run `Jogo → 3ds Max → Macros → Jogo`. Triggering either while on
layer 3 or 4 returns to Jogo.

Rules enforced by the chord (`totem.c`):

- both keys must go down within `PROFILE_CHORD_SYNC_MS` (50 ms) of each other;
- the chord must be held `PROFILE_SWITCH_HOLD_MS` (1000 ms);
- while armed, neither key's keycode reaches the host, so a recognised switch
  never leaks a stray `G` or `B` into the game;
- if released early, both keys are delivered normally.

The selected profile is written to EEPROM, so **it survives a reboot**.

> **Caveat:** the two chord keys are withheld for up to 50 ms and replayed with
> `register_code16()`. Keep them on *basic* keycodes — layer taps, tap dance
> and Vial macros on those two positions will not replay correctly.

## Encoder and OLED menu

```
short click       -> next profile        (in any state)
turn              -> navigate / adjust
long click 500 ms -> open menu, enter item, confirm, go back
```

The short click is reserved for the profile, so the menu uses the **long
click** to confirm — otherwise there would be no way to enter an item.

Menu items: Perfil · Tela · Brilho RGB · Efeito RGB · Encoder · Cronometro ·
Zerar tempo · Reiniciar.

Idle screens, selected under *Tela*: **Status** (profile, encoder mode, RGB,
APM, stopwatch), **APM** (rolling 60 s window with a bar), **Tempo**
(stopwatch), **Logo**.

The *Encoder* item switches the knob between `Menu` (default: turning opens
and drives the menu) and `Teclas` (turning sends volume up/down; the menu is
still reachable by long click).

The panel blanks after 60 s idle (`OLED_TIMEOUT`) because OLEDs burn in. Any
key or turn wakes it.

> **Two deliberate limitations.**
> `ENCODER_MAP_ENABLE` is **off**: with it on, QMK dispatches encoder events
> straight to keycodes and never calls `encoder_update_kb()`, which is where
> the menu reads the rotation. So the encoder **rotation is not remappable in
> Vial** — the menu owns it. To give it a different fallback job, change the
> two keycodes in `encoder_update_kb()` in `totem.c`.
> The encoder **button** is likewise intercepted by matrix position, so its
> keycode in the keymap is never sent; remapping `[0,5]` in Vial has no effect.

## RGB

`RGB_MATRIX` over the WS2812 vendor (PIO) driver: 20 LEDs, one per key.

**The chain must be routed DIN → DOUT following SW1..SW20** — the same order as
the `LAYOUT` macro: top row left to right, then each row below. That mapping
lives in `g_led_config` in `totem.c`; if the PCB serpentines instead, only the
first block of that struct changes.

- Profile colour sets only the **hue**; the effect and brightness you pick in
  Vial or the OLED menu are left alone.
- All indicator writes use `_noeeprom`, so they never overwrite saved settings
  nor wear the flash.
- `max_brightness` is capped at 120: 20 WS2812 at full white draw well over
  1 A, far past the 500 mA USB budget. Typical use — a solid profile colour at
  brightness 80 — is around 120-150 mA.

> **Level shifting:** `GP26` drives 3.3 V, but a WS2812B on 5 V wants
> 0.7 x VDD = 3.5 V for a logic high. It usually works, but if the first LED
> misbehaves, use SK6812 instead, drop the LED supply to ~4.3 V with a series
> diode, or add a 74AHCT125.

## Building from scratch

From nothing installed to a `.uf2`. Verified on macOS/arm64, August 2026.

This repository is a hardware project, not a QMK tree: it has no build system.
The `totem/` folder is only the *keyboard definition*, which has to sit inside
a QMK tree supplying `quantum/`, ChibiOS, the pico-sdk and the Makefiles
(~1.6 GB — deliberately not vendored here).

The Vial fork of QMK is required; this will **not** build against upstream
`qmk_firmware`.

### 1. Python 3.11

Not fussiness: QMK's build scripts use `ast.Num`, removed in Python 3.12. On
3.12 or newer the build dies with
`AttributeError: module 'ast' has no attribute 'Num'`.

```sh
brew install python@3.11
```

### 2. Clone vial-qmk

```sh
git clone --recurse-submodules https://github.com/vial-kb/vial-qmk.git ~/vial-qmk
```

`--recurse-submodules` is mandatory: without it ChibiOS and the pico-sdk are
missing and the build fails much later without saying why.

### 3. Python environment

```sh
/opt/homebrew/opt/python@3.11/bin/python3.11 -m venv ~/vial-qmk/.venv
~/vial-qmk/.venv/bin/pip install --upgrade pip
~/vial-qmk/.venv/bin/pip install -r ~/vial-qmk/requirements.txt qmk
```

### 4. ARM toolchain

It must include **newlib**. Homebrew's `arm-none-eabi-gcc` does **not** — it
ships without the libc headers and the build dies on
`fatal error: stdint.h: No such file or directory`.

**Path A — official cask** (asks for a sudo password):

```sh
brew install --cask gcc-arm-embedded
```

**Path B — no sudo**, extracting the same `.pkg`. This is how this firmware was
actually built:

```sh
brew fetch --cask gcc-arm-embedded
PKG=$(find ~/Library/Caches/Homebrew/downloads -name "*arm-gnu-toolchain*.pkg" | head -1)

mkdir -p /tmp/armx && cd /tmp/armx
xar -xf "$PKG"
mkdir -p out && cat Payload | gzip -dc | cpio -idm -D out

mv out ~/vial-qmk/.toolchain
~/vial-qmk/.toolchain/bin/arm-none-eabi-gcc --version   # check
```

### 5. Link the keyboard definition

```sh
cd <this-repository>
git checkout encoder-oled-screen
ln -s "$(pwd)/firmware/QMK/VIAL_source/totem" ~/vial-qmk/keyboards/totem
```

Symlink, **never a copy**. Copying makes two versions of the same files: you
edit in the repo, forget to copy, build the stale one, and cannot work out why
your change did nothing.

### 6. Build

```sh
export PATH="$HOME/vial-qmk/.toolchain/bin:$HOME/vial-qmk/.venv/bin:$PATH"
cd ~/vial-qmk

make totem:vial       # -> totem_vial.uf2    125 KiB · recommended
make totem:default    # -> totem_default.uf2  89 KiB · plain QMK
```

The `export PATH` must come **before** `make`, in every new shell. A clean
build ends in `Creating UF2 file for deployment` and `[OK]`. Any `[ERRORS]` is
a real failure — QMK treats warnings as errors, so "built with warnings" does
not exist.

> **A trap that misleads:** `brew install qmk/qmk/qmk` can exit **0 without
> installing anything**, blocked by Homebrew's tap-trust policy — the output
> says `Would install 1 formula` and stops. The steps above sidestep it by not
> depending on that tap.
>
> **Another one:** a `//` comment key placed *inside* a nested object in
> `keyboard.json` fails QMK's schema validation, and the build reports it as
> `No bootloader specified` — nowhere near the actual cause. Comment keys only
> work at the top level.

### Rebuilding after a change

```sh
export PATH="$HOME/vial-qmk/.toolchain/bin:$HOME/vial-qmk/.venv/bin:$PATH"
cd ~/vial-qmk && make totem:vial
```

Steps 1–5 are one-time. If something behaves oddly after editing
`keyboard.json` or `rules.mk`, clear the cache with `rm -rf ~/vial-qmk/.build`.

## Flashing

1. Double-tap RESET. The board mounts as a USB drive named `RPI-RP2`.
2. Copy `totem_vial.uf2` onto it.
3. It reboots into the new firmware and the drive disappears.

If `RPI-RP2` does not appear: hold **BOOT**, press and release **RESET**, then
release **BOOT**.

## Vial

`VIAL_KEYBOARD_UID` is freshly generated and deliberately different from the
upstream TOTEM's — Vial caches keyboard definitions by UID, and reusing it
would make Vial load the wrong 38-key layout.

The security unlock combo is **"1" + "R"** (`[0,1]` and `[0,4]`).
