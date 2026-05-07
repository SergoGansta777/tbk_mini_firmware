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
- right thumb center: `Caps Lock` on tap, `System` on hold

`HOLD_ON_OTHER_KEY_PRESS_PER_KEY` is enabled only for those two keys. This makes
the layer decision settle earlier when you chord the thumb key with another key,
which is better for fast typing than waiting out the full tapping term.

Additional tuning for the two thumb layer keys:

- `Nav/Tab` uses a `160ms` tapping term
- `System/Caps Lock` uses a `170ms` tapping term
- `QUICK_TAP_TERM` is disabled for both keys so a fast tap-then-hold does not
  repeat `Tab` or `Caps Lock` instead of entering the layer

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
- right: `Enter`, `System/Caps Lock`, `Backspace`

The two momentary layer keys sit in the middle thumb positions on both halves.

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
- left home row: cut, undo, delete previous word, copy, paste, kill to end
- right home row: arrows on `H J K L`
- right edge: `Repeat` and `Alt Repeat`
- bottom left: previous/next word, line start/end, find
- bottom right: `Home`, `Page Down`, `Page Up`, `End`, previous/next tab

This layer is intentionally biased toward movement and text editing rather than
general application shortcuts. On macOS, `Option-Delete` removes the previous
word and `Control-K` deletes from the cursor to the end of the line or
paragraph, which makes them better Nav-layer residents than `Cmd-A` or a
dedicated `Redo` slot for this workflow.

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
- toggle combos

This keeps maintenance actions off normal daily layers.

## RGB status indicators

The firmware uses a small set of LED indicators instead of full-board layer
lighting:

- `Nav` layer: after a short hold delay, the `Nav/Tab` thumb LED turns cyan and the opposite layer thumb glows dim cyan
- `System` layer: after a short hold delay, the `System/Caps Lock` thumb LED turns amber and the opposite layer thumb glows dim amber
- `Keyboard` layer: both layer-thumb LEDs turn red
- `Caps Word`: both shift LEDs turn green
- combos disabled on the base layer: both layer-thumb LEDs turn purple

Momentary layer indicators use lower brightness and a small delay so quick
number-entry and short nav taps do not flash in peripheral vision.

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
- a `26ms` floor for all custom per-combo timings
- slightly stricter per-combo timings for common same-hand rolls
- a small bump for `(`
- a tiny bump for `-`
- a slightly looser timing for `{`, with `}` kept a bit tighter
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

The following combos stay below the `30ms` default because they overlap with
plausible same-hand rolls, but none of them now go below `26ms`:

- `E + R`
- `U + I`
- `I + O`
- `S + D`
- `K + L`

`W + E -> -` is still kept below the `30ms` default, but gets a small extra
margin because it is a high-value programming combo.

Quote combos now use the same `26ms` floor:

- `D + F`
- `J + K`

`R + T -> (` is slightly looser than the quote combos, but still below the
`30ms` default to keep accidental `rt` triggers in check.

Curly brace combos are slightly looser than the `30ms` default because they are
harder to chord cleanly on the TBK Mini's outer curl. The opening `{` gets a
little more room than `}` because it is the more valuable and harder-to-hit
chord:

- `V + B`
- `N + M`

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
