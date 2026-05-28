# `reshka` TBK Mini keymap

This keymap is a documented starting point for a TBK Mini on Splinky v3 using:

- BastardKB `bkb-develop` as the firmware base
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

## Modern stack baseline

This keymap now assumes the post-migration BastardKB / QMK stack:

- explicit hardware target: `bastardkb/tbkmini/splinktegrated_rev1`
- shared TBK Mini keymap path: `keyboards/bastardkb/tbkmini/keymaps/reshka`
- built-in `SPLIT_LAYER_STATE_ENABLE` for layer-aware split cosmetics
- built-in `SPLIT_ACTIVITY_ENABLE` so idle RGB timeouts respect either half
- built-in `SPLIT_WATCHDOG_ENABLE` for split-link recovery
- no forced `KEYBOARD_SHARED_EP`, since the modern build no longer needs it

The only remaining custom split RPC is the small RGB-indicator payload for
`Caps Word` and combo-enabled state, because those are not mirrored by QMK's
core split sync.

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

While Caps Word is active, holding either Shift key inverts case instead of
breaking the word. This makes mixed-case suffixes like `DBaaS` and `PDFs`
possible without leaving Caps Word.

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

- left: `Globe/Fn`, `Nav/Caps Lock`, `Space`
- right: `Enter`, `Num + System/Tab`, `Backspace`

The two momentary layer keys sit in the middle thumb positions on both halves.
The left outer thumb is an experimental macOS Globe/Fn key implemented with
Apple's keyboard-layout-select consumer usage. `Num + System` still exposes a
plain `Gui/Command` on that same physical key.

The base layer intentionally has no firmware home-row mods.

### Layer 1: Nav

Purpose:

- vim-inspired navigation and search
- `H J K L` arrows
- repeat and alternate repeat as motion helpers
- a small set of high-value edit actions
- fallback modifiers for mouse and window workflows
- layer lock

Layout logic:

- `H J K L` = arrows
- `W` and `E` = next word
- `B` = previous word
- `A` = line end
- `I` = line start
- `G` = document end
- `/` = find
- `Shift + /` = global / project search when using a real firmware Shift or one-shot Shift
- `N` = next search result
- `;` = `Repeat`
- `'` = `Alt Repeat`
- `Y`, `U`, `P` = copy, undo, paste
- `S` = delete previous word
- `D` = kill to end of line / paragraph
- left outer column = `Ctrl`, `Alt`, `Cmd` as one-shot modifiers
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
- `Alt Repeat` after undo gives redo

This avoids pretending that generic macOS text fields can provide exact Vim
semantics for things like distinct `w` vs `e` or true `0` vs `^`.

### Layer 2: Num + System

Purpose:

- top-row numbers for programming and counts
- `Delete`
- media controls
- brightness
- `F1..F12`
- layer lock

This layer is intentionally limited to machine-level controls: function keys,
media, display brightness, and the number row on the right-thumb layer, so
`Nav` can stay focused on movement instead of carrying both jobs at once.

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
- left outer column = fallback modifiers
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
- `Shift + /` = global / project search when using a real firmware Shift or one-shot Shift
- `N` = next search result
- `'` after `N` = previous search result
- `'` after `/` = global / project search
- top row second key = `Layer Lock` with a 60-second idle timeout

Fallback modifier flow:

- top-left outer key = `OSM(Ctrl)`
- left home outer key = `OSM(Alt)`
- left bottom outer key = `OSM(Gui / Cmd)`
- tap one of them for a sticky modifier on the next key
- hold one of them for a normal held modifier while using the mouse or trackpad

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
- `U`, then `'` = redo

Outside `Nav`, `Alt Repeat` also complements a couple of high-value editing keys:

- `Tab`, then `Alt Repeat` = `Shift + Tab`
- `Shift + Tab`, then `Alt Repeat` = `Tab`

Repeat is intentionally motion-biased on this layer:

- `Delete`, `Copy`, and `Paste` do not replace the remembered motion/search key
- `Undo` still stays repeatable so `Alt Repeat` can complement it with redo
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
- left thumbs = `Gui`, `Shift`, `Space`

Practical layout:

- top row = `1 2 3 4 5 6 7 8 9 0`
- top-right outer key = `Delete`
- top-left outer key = `Layer Lock` with a 60-second idle timeout
- left home row = brightness plus four intentionally empty spare slots
- right home row = previous, play/pause, next, mute, volume down, volume up
- bottom row = `F1..F12` in order
- left outer thumb = real `Gui/Command` while this layer is held
- left middle thumb = `Shift` for shifted number-row symbols
- left inner thumb = `Space`

This keeps numbers and shifted number symbols natural for programming while
keeping system controls on the same deliberate right-thumb layer, while still
giving you a plain `Cmd` key when you need one inside the utility layer.

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
the slave half so both sides render the same status LEDs. Layer state now uses
QMK's built-in split sync, while `Caps Word` and combo-enabled state stay on a
small custom user RPC.

Momentary layer indicators use lower brightness and a small delay so quick
number-entry and short nav taps do not flash in peripheral vision.

RGB Matrix also turns off after `15` minutes of inactivity, and that timeout is
fed by activity from either half of the split board.

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
- same-hand combos kept only where they are clearly worth it
- a small bump for `(`
- a dedicated slightly looser timeout for `F + J`
- the loosest timeout reserved for `D + K`
- a slightly looser timing for `{`, with `}` kept a bit tighter

This keeps the combo set usable for fast typing while reducing accidental
triggers from normal rolls.

### Programming combos

- `R + T -> (`
- `Y + U -> )`
- `F + G -> [`
- `H + J -> ]`
- `V + B -> {`
- `N + M -> }`
- `D + F -> -`
- `D + K -> _`
- `F + J -> =`
- `J + K -> +`
- `S + D -> backtick`

### Operator mnemonic

The operator combos now use the freed home-row slots and keep the two shifted
variants close to their base symbol:

- `D + F -> -`
- `D + K -> _`
- `F + J -> =`
- `J + K -> +`

This keeps `-` and `+` on easy adjacent same-hand chords, while `_` and `=` stay
on their stronger dedicated shapes.

### Combo timing notes

Only two combos stay meaningfully tighter than the `30ms` default:

- `S + D`
- `R + T`

`R + T -> (` is kept below the
`30ms` default to keep accidental `rt` triggers in check.

Curly brace combos are slightly looser than the `30ms` default because they are
harder to chord cleanly on the TBK Mini's outer curl. The opening `{` gets a
little more room than `}` because it is the more valuable and harder-to-hit
chord:

- `V + B`
- `N + M`

The cross-hand operator box gets extra room because those operator chords are
more deliberate and physically cleaner on this board, but they no longer share
one generic timing bucket:

- `D + K`
- `F + J`

`D + K -> _` is the loosest of the set because it is the hardest one to hit
cleanly and is also a high-value programming symbol.

## Build

After `user.qmk_home` points at the BastardKB firmware tree and
`user.overlay_dir` points at this userspace, the preferred build command is:

```sh
qmk userspace-compile -c
```

Equivalent explicit target:

```sh
qmk compile -kb bastardkb/tbkmini/splinktegrated_rev1 -km reshka
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
