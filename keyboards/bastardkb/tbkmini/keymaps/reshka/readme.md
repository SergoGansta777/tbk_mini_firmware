# `reshka` TBK Mini keymap

This keymap is a documented starting point for a TBK Mini on Splinky v3 using:

- BastardKB `bkb-master` as the firmware base
- external QMK userspace for custom keymaps
- VIA for live remapping after the firmware structure is stable
- macOS host-side home-row mods, not firmware home-row mods

## Design goals

1. Keep the base layer simple and predictable.
2. Keep firmware responsibilities separate from host-side home-row mods.
3. Use a small number of modern QMK features with clear value.
4. Make navigation and system control efficient without creating extra layers.

## Enabled features

- `VIA_ENABLE`
- `CAPS_WORD_ENABLE`
- `COMBO_ENABLE`
- `KEY_OVERRIDE_ENABLE`
- `LAYER_LOCK_ENABLE`
- `REPEAT_KEY_ENABLE`

## Tap-hold behavior

Only the two thumb layer-taps use tap-hold behavior:

- left thumb center: `Tab` on tap, `Nav` on hold
- right thumb outer: `Caps Lock` on tap, `System` on hold

`HOLD_ON_OTHER_KEY_PRESS_PER_KEY` is enabled only for those two keys. This makes
the layer decision settle earlier when you chord the thumb key with another key,
which is better for fast typing than waiting out the full tapping term.

## Layer overview

### Layer 0: Base

Left outer column, bottom to top:

- `Left Shift`
- `Escape`
- `Tab`

Right outer column, bottom to top:

- `Right Shift`
- `Semicolon`
- `Backslash`

Thumbs:

- left: `Command`, `Nav/Tab`, `Space`
- right: `Enter`, `Backspace`, `System/Caps Lock`

The base layer intentionally has no firmware home-row mods.

### Layer 1: Nav + Num

Purpose:

- numbers on the top row
- `H J K L` arrows
- macOS editing shortcuts
- repeat and alternate repeat
- layer lock

Layout logic:

- top row: `1..0` and `Delete`
- left hand: select all, undo, cut, copy, paste, redo
- right home row: arrows on `H J K L`
- right edge: `Repeat` and `Alt Repeat`
- bottom left: previous/next word, line start/end, find
- bottom right: `Home`, `Page Down`, `Page Up`, `End`, previous/next tab

### Layer 2: System

Purpose:

- `F1..F12`
- media controls
- brightness
- desktop switching and Spotlight on macOS
- screenshot shortcut
- layer lock

### Layer 3: Keyboard

This is a tri-layer that activates only while both `Nav` and `System` are held.

Purpose:

- bootloader access
- EEPROM clear
- RGB matrix controls
- toggle key overrides
- fallback `Caps Word` toggle

This keeps maintenance actions off normal daily layers.

## Key overrides

- `Shift + Backspace -> Delete`
- `Shift + Escape -> ~`

The second override gives easy access to `~` without introducing a dedicated
symbol layer.

## Combos

Combos are intentionally configured with a strict policy:

- base layer only
- tap-only
- `COMBO_STRICT_TIMER`
- `30ms` default timing
- stricter per-combo timings for common same-hand bigrams
- slightly looser timing for the cross-hand `F + J` combo

This keeps the combo set usable for fast typing while reducing accidental
triggers from normal rolls.

### Programming combos

- `R + T -> (`
- `Y + U -> )`
- `F + G -> [`
- `H + J -> ]`
- `V + B -> {`
- `N + M -> }`
- `F + J -> =`
- `W + E -> -`
- `E + R -> +`
- `U + I -> _`
- `I + O -> |`
- `D + F -> '`
- `J + K -> "`
- `S + D -> backtick`
- `K + L -> ~`

### Combo timing notes

The following combos are stricter than the `30ms` default because they overlap
with more plausible same-hand letter rolls:

- `W + E`
- `E + R`
- `U + I`
- `I + O`
- `S + D`
- `K + L`

Quote combos are also slightly stricter:

- `D + F`
- `J + K`
- `R + T`

`F + J -> =` is slightly looser because it is a same-position cross-hand combo
and is much less likely to appear accidentally in normal typing.

## Build

After `user.qmk_home` points at the BastardKB firmware tree and
`user.overlay_dir` points at this userspace:

```sh
qmk compile -kb bastardkb/tbkmini -km reshka
```

## Flash

For Splinky v3:

1. Disconnect the halves from each other.
2. Plug in one half by USB.
3. Double-press reset to enter the UF2 bootloader.
4. Copy the generated `.uf2` onto the mounted drive.
5. Repeat for the other half.
