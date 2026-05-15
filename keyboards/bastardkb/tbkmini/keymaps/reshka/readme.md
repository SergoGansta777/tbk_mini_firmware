# `reshka` TBK Mini keymap

This keymap is a documented starting point for a TBK Mini on Splinky v3 using:

- BastardKB `bkb-master` as the firmware base
- external QMK userspace for custom keymaps
- VIA for live remapping after the firmware structure is stable
- macOS host-side home-row mods, not firmware home-row mods

The root repo README is the recovery and operations guide. This file is the
behavior and design guide for the current keymap.

## Design goals

1. Keep the base layer simple and predictable.
2. Keep firmware responsibilities separate from host-side home-row mods.
3. Use a small number of modern QMK features with clear value.
4. Keep the layer model small while making navigation and system control more coherent.

## Enabled features

- `VIA_ENABLE`
- `CAPS_WORD_ENABLE`
- `COMBO_ENABLE`
- `KEY_OVERRIDE_ENABLE`
- `LAYER_LOCK_ENABLE`
- `REPEAT_KEY_ENABLE`

## Home-Row Mods Policy

This keymap intentionally keeps the alpha home row plain.

Current policy:

- home-row mods are handled by the macOS HRM app, not by QMK
- firmware does not use `MT()` or other alpha home-row tap-hold behavior
- firmware-only modifier features see only real firmware modifiers

Practical consequence:

- `Shift + Backspace -> Delete` works only with a real firmware Shift key
- `Shift + Escape -> ~` also works only with a real firmware Shift key
- a host-generated Shift from the macOS HRM app is not visible to QMK as a
  physical modifier press

## Tap-hold behavior

Only the two thumb layer-taps use tap-hold behavior:

- left thumb center: `Caps Lock` on tap, `Nav` on hold
- right thumb center: `Tab` on tap, `Num + System` on hold

`HOLD_ON_OTHER_KEY_PRESS_PER_KEY` is enabled only for those two keys. This makes
the layer decision settle earlier when you chord the thumb key with another key,
which is better for fast typing than waiting out the full tapping term.

Additional tuning for the two thumb layer keys:

- `Nav/Caps Lock` uses a `160ms` tapping term
- `Num + System/Tab` uses a `170ms` tapping term
- `QUICK_TAP_TERM` is disabled for both keys so a fast tap-then-hold does not
  repeat `Caps Lock` or `Tab` instead of entering the layer

Caps Word is entered by pressing both Shift keys together.

## Layer overview

### Layer 0: Base

Left outer column, bottom to top:

- `Left Shift`
- `Escape`
- `Tab`

Right outer column, bottom to top:

- `Right Shift`
- `Quote`
- `Backslash`

Thumbs:

- left: `Command`, `Nav/Caps Lock`, `Space`
- right: `Enter`, `Num + System/Tab`, `Backspace`

The two momentary layer keys sit in the middle thumb positions on both halves.

The base layer intentionally has no firmware home-row mods.

### Layer 1: Nav

Purpose:

- vim-inspired navigation and search
- `H J K L` arrows
- repeat and alternate repeat as motion helpers
- a small set of high-value edit actions
- layer lock

Layout logic:

- `H J K L` = arrows
- `W` and `E` = next word
- `B` = previous word
- `A` = line end
- `I` = line start
- `G` = document end
- `/` = find
- `Shift + /` = global / project search when using a real firmware Shift
- `N` = next search result
- `;` = `Repeat`
- `'` = `Alt Repeat`
- `Y`, `U`, `P` = copy, undo, paste
- `S` = delete previous word
- `D` = kill to end of line / paragraph
- `,` and `.` = page up / page down

This layer is intentionally vim-inspired, not a full Vim emulator. It borrows
the strongest motion/search mnemonics that map cleanly to macOS shortcuts and
leans on `Repeat` / `Alt Repeat` to supply the opposite direction where that is
cleaner than spending extra keys.

Important approximations:

- `W` and `E` both map to forward word motion
- `Alt Repeat` after `/` gives global / project search
- `Alt Repeat` after `W` or `E` gives backward word motion
- `Alt Repeat` after `N` gives previous search result
- `Alt Repeat` after `A` gives line start
- `Alt Repeat` after `G` gives document start

This avoids pretending that generic macOS text fields can provide exact Vim
semantics for things like distinct `w` vs `e` or true `0` vs `^`.

### Layer 2: Num + System

Purpose:

- top-row numbers for programming and counts
- `Delete`
- media controls
- brightness
- Spotlight and Mission Control on macOS
- screenshot shortcut
- `F1..F12`
- layer lock

This layer is intentionally limited to machine-level controls: function keys,
media, display brightness, and a small set of macOS workspace actions, with the
number row restored to the right-thumb layer so `Nav` can stay focused on
movement instead of carrying both jobs at once.

### Layer 3: Keyboard

This is a tri-layer that activates only while both `Nav` and `Num + System` are
held.

Purpose:

- bootloader access
- EEPROM clear
- RGB matrix toggle
- toggle key overrides
- toggle combos

This keeps maintenance actions off normal daily layers.

## Layer usage guide

### Base layer guide

Use the base layer as a normal plain QWERTY layer.

Important habits:

- the alpha home row is intentionally dumb and reliable
- left middle thumb = tap `Caps Lock`, hold `Nav`
- right middle thumb = tap `Tab`, hold `Num + System`
- `Shift + Backspace -> Delete` and `Shift + Escape -> ~` require a real
  firmware Shift key, not a host-generated HRM Shift

This layer is meant to stay low-drama so the more advanced behavior lives only
on deliberate layer holds and combos.

### Nav layer guide

Hold the left middle thumb key to enter `Nav`.

How to think about it:

- right hand = cursor movement
- left hand = word, line, document, and edit actions
- `;` and `'` = continue or reverse the last motion idea

Core motions:

- `H J K L` = left, down, up, right
- `W` or `E` = next word
- `B` = previous word
- `I` = line start
- `A` = line end
- `G` = document end
- `,` / `.` = page up / page down

Search flow:

- `/` = find
- `Shift + /` = global / project search when using a real firmware Shift
- `N` = next search result
- `'` after `N` = previous search result
- `'` after `/` = global / project search
- top-left outer key = `Layer Lock`

Selection flow:

- hold `Shift`, then use `H J K L` to extend by character / line direction
- hold `Shift`, then use `W` or `E` to extend by next word
- hold `Shift`, then use `B` to extend by previous word
- hold `Shift`, then use `I` / `A` to extend to line start / line end
- hold `Shift`, then use `G` to extend to document end
- hold `Shift`, then use `,` / `.` to extend by page where the app supports it
- keep holding `Shift`, then use `;` / `'` to continue or reverse the last motion

Edit helpers:

- `U` = undo
- `Y` = copy
- `P` = paste
- `S` = delete previous word
- `D` = kill to end of line / paragraph
- top-right outer key = `Delete`

Repeat flow:

- `;` = `Repeat`
- `'` = `Alt Repeat`

This is what makes the layer feel more Vim-like without becoming modal:

- `W`, then `'` = previous word
- `A`, then `'` = line start
- `G`, then `'` = document start
- `N`, then `'` = previous search result

Repeat is intentionally motion-biased on this layer:

- `Delete`, `Copy`, and `Paste` do not replace the remembered motion/search key
- this keeps `;` and `'` acting more like navigation/search operators than
  clipboard helpers

Important limitation:

- this is a generic macOS navigation layer, not true Vim state
- it intentionally does not try to fake exact `^`, text objects, or operators
- `W` and `E` are intentionally collapsed to the same forward-word action
- if host-side HRM Shift does not combine reliably with `Nav` `/`, use `/` then
  `Alt Repeat` instead of relying on `Shift + /`

### Num + System layer guide

Hold the right middle thumb key to enter `Num + System`.

How to think about it:

- top row = real number row
- left side = machine controls
- right side = media
- bottom row = full `F1..F12`

Practical layout:

- top row = `1 2 3 4 5 6 7 8 9 0`
- top-right outer key = `Delete`
- top-left outer key = `Layer Lock`
- left home row = brightness, Spotlight, Mission Control, screenshot
- right home row = previous, play/pause, next, mute, volume down, volume up
- bottom row = `F1..F12` in order

This keeps numbers and shifted number symbols natural for programming while
keeping system controls on the same deliberate right-thumb layer.

### Keyboard layer guide

Hold both middle thumb layer keys together to enter `Keyboard`.

This layer is only for maintenance:

- base `Tab` position = `QK_BOOT`
- base `Q` position = `EE_CLR`
- base `Esc` position = RGB toggle
- base `;` position = key override toggle
- base `'` position = combo toggle

Use it when:

- you need bootloader entry without touching the controller button
- VIA remaps survived a reflash and you need `EE_CLR`
- you want to temporarily disable combos or key overrides while debugging
- you want to toggle RGB entirely on or off

## RGB status indicators

The firmware uses a small set of LED indicators instead of full-board layer
lighting:

- `Nav` layer: after a short hold delay, only the `Nav/Caps Lock` thumb LED turns blue/cyan
- `Num + System` layer: after a short hold delay, the `Num + System/Tab` thumb LED turns amber and the opposite layer thumb glows dim amber
- `Keyboard` layer: both layer-thumb LEDs turn red
- `Caps Word`: both shift LEDs turn green
- combos disabled on the base layer: both layer-thumb LEDs turn purple

On this split build, layer, `Caps Word`, and combo-indicator state are synced to
the slave half so both sides render the same status LEDs.

Momentary layer indicators use lower brightness and a small delay so quick
number-entry and short nav taps do not flash in peripheral vision.

## Key overrides

- `Shift + Backspace -> Delete`
- `Shift + Escape -> ~`

The second override gives easy access to `~` without introducing a dedicated
symbol layer, so a separate `~` combo is intentionally omitted.

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

### Combo timing notes

The following combos stay below the `30ms` default because they overlap with
plausible same-hand rolls, but none of them now go below `26ms`:

- `E + R`
- `U + I`
- `I + O`
- `S + D`

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

## EEPROM and VIA

VIA remaps are stored in EEPROM, so they survive a normal reflash.

If a flashed default layout does not seem to apply:

1. hold both middle thumb layer keys to enter `Keyboard`
2. press the physical `Q` position on the left half
3. this triggers `EE_CLR` and resets persistent EEPROM state

## Where To Change Things Later

- `keymap.c`
  - layers, combos, key overrides, RGB indicators, and custom behavior
- `config.h`
  - tapping terms, combo timing defaults, Caps Word options, and core feature flags
- `rules.mk`
  - QMK feature enables
- root `README.md`
  - repo setup, recovery, build, and flash workflow

## Future Work Notes

If home-row mods ever move into firmware, treat that as a separate design
project. Do not mix host-side HRMs and firmware HRMs at the same time in daily
use, because it makes debugging tap-hold behavior and modifier-dependent
features much harder.
