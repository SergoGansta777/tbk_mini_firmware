# TBK Mini QMK Userspace

This repository is the source of truth for the custom firmware of a BastardKB
TBK Mini with Splinky v3 controllers.

The goal of this repo is long-term maintainability:

- keep personal firmware logic out of the vendor QMK tree
- make recovery from a fresh machine straightforward
- document the current keyboard behavior and design choices
- keep build and flash steps reproducible

The actual custom keymap lives at:

- `keyboards/bastardkb/tbkmini/keymaps/reshka`

The build target is:

- `bastardkb/tbkmini/splinktegrated_rev1:reshka`

On newer BastardKB branches, TBK Mini is revisioned. The build target is
`splinktegrated_rev1` because it matches the older GP1 soft-serial RP2040 TBK
Mini definition that this repo was already building against. The keymap itself
stays in the shared `tbkmini/keymaps/` directory, which matches BastardKB's own
layout for revisioned TBK Mini targets.

## Hardware and Current Model

- keyboard: BastardKB TBK Mini
- controllers: Splinky v3 on both halves
- firmware base: `bastardkb-qmk`
- vendor branch: local `bkb-develop` tracking `origin/bkb-develop`
- custom logic: this external userspace repo
- runtime remapping: VIA
- home-row mods: currently handled on macOS by a host-side app, not in firmware

Modern stack notes:

- build target: `bastardkb/tbkmini/splinktegrated_rev1`
- keymap path stays shared at `keyboards/bastardkb/tbkmini/keymaps/reshka`
- split layer state and split activity use QMK's built-in sync
- a small custom split RPC remains only for host `Caps Lock`, `Caps Word`, and combo-indicator state
- `KEYBOARD_SHARED_EP = yes` stays enabled, because this setup needs it for reliable macOS Globe/Fn hold shortcuts

Important consequence:

- firmware-side modifier overrides only see real firmware modifiers
- host-generated modifiers from the macOS HRM app are not visible to QMK

## Repositories

This setup uses two repositories:

1. Vendor firmware base
   - expected repo: `https://github.com/Bastardkb/bastardkb-qmk.git`
   - expected branch: `bkb-develop`

2. Personal userspace
   - this repository
   - stores the keymap, feature config, docs, and GitHub Actions workflow

The vendor repo can always be recloned. This userspace repo is the important
backup.

Known local upstream state when this README was written:

- firmware repo upstream remote: `origin -> https://github.com/Bastardkb/bastardkb-qmk.git`
- known-good firmware base commit: `a02692a7887df12ad026b3e1085c1b890a3902ef`

If BastardKB changes something upstream and a future build behaves differently,
you can temporarily check out that exact commit in the vendor repo to reproduce
the previously tested state.

## Repository Layout

- `README.md`
  - machine-recovery, setup, build, and flash guide
- `qmk.json`
  - userspace build targets for QMK and GitHub Actions
- `.github/workflows/build_binaries.yaml`
  - builds firmware on push using BastardKB QMK on `bkb-develop`
- `keyboards/bastardkb/tbkmini/keymaps/reshka/keymap.c`
  - layers, combos, key overrides, RGB indicators, and custom behavior
- `keyboards/bastardkb/tbkmini/keymaps/reshka/config.h`
  - tapping, combo, and feature config
- `keyboards/bastardkb/tbkmini/keymaps/reshka/rules.mk`
  - feature enables
- `keyboards/bastardkb/tbkmini/keymaps/reshka/readme.md`
  - detailed design notes for the current keymap

## Cold-Start Recovery

If your computer breaks or you reinstall everything, recover in this order.

### 1. Install QMK CLI

Install the QMK CLI and its normal build dependencies for your platform.

Then verify the environment:

```sh
qmk doctor
```

### 2. Clone the vendor firmware repo

Example:

```sh
git clone https://github.com/Bastardkb/bastardkb-qmk.git ~/src/bastardkb-qmk
cd ~/src/bastardkb-qmk
git switch --track origin/bkb-develop
```

If you need the known-tested upstream snapshot instead of current `bkb-develop`:

```sh
git checkout a02692a7887df12ad026b3e1085c1b890a3902ef
```

### 3. Clone this userspace repo

Example:

```sh
git clone git@github.com:<your-user>/<your-userspace-repo>.git ~/src/bastardkb-qmk-userspace
```

### 4. Point QMK at both repos

Set QMK to use the BastardKB repo as `qmk_home` and this repo as the overlay:

```sh
qmk config user.qmk_home="$HOME/src/bastardkb-qmk"
qmk config user.overlay_dir="$HOME/src/bastardkb-qmk-userspace"
```

Current local values on the machine that produced this repo:

```sh
user.qmk_home=/Users/0xse.reshka/Projects/Other/bastardkb-qmk
user.overlay_dir=/Users/0xse.reshka/Projects/Other/bastardkb-qmk-userspace
```

### 5. Build

Preferred userspace entrypoint:

```sh
qmk userspace-compile -c
```

Run QMK builds sequentially. The vendor tree uses a shared `.build` directory,
so overlapping local compiles can trip over each other.

Equivalent explicit target:

```sh
qmk compile -c -kb bastardkb/tbkmini/splinktegrated_rev1 -km reshka
```

The generated UF2 is copied to the userspace root:

- `bastardkb_tbkmini_splinktegrated_rev1_reshka.uf2`

### 6. Flash both halves

Use the same UF2 on both halves.

Recommended process:

1. Build the firmware.
2. Disconnect the halves from each other to reduce variables while flashing.
3. Plug USB into one half.
4. Double-press the bottom `UPDATE` button to enter the UF2 bootloader.
5. Copy `bastardkb_tbkmini_splinktegrated_rev1_reshka.uf2` to the `RPI-RP2` drive.
6. Repeat on the other half with the same UF2.
7. Reconnect the halves and use USB on the right half.

### 7. Reset VIA and EEPROM if needed

VIA remaps survive a normal reflash because they are stored in EEPROM.

If the flashed default layout does not match what the board does:

1. Hold both middle thumb layer keys to enter the `Keyboard` tri-layer.
2. Press the left-half `Q` position.
3. This triggers `EE_CLR` and wipes persistent EEPROM state, including VIA remaps.

## Daily Workflow

When making changes later, use this order:

1. Edit source in this userspace repo.
2. Update docs if behavior changed.
3. Compile.
4. Flash both halves with the same UF2.
5. Test the changed behavior.
6. Commit in this userspace repo.

## What To Edit

Use these files deliberately:

- `keymap.c`
  - change layers, combos, overrides, indicators, and behavior
- `config.h`
  - change tapping terms, combo settings, and core feature flags
- `rules.mk`
  - enable or disable QMK features
- `readme.md` inside the keymap directory
  - update behavior and rationale when the keymap changes
- `README.md` in the repo root
  - update recovery, build, and flash instructions
- `qmk.json`
  - update build targets if you add more keyboards or keymaps

## Current Workflow Rules

These are intentional project rules, not accidents:

1. Do not keep personal keymap logic in the vendor firmware repo.
2. Keep BastardKB QMK as the base and this repo as the custom layer.
3. Keep home-row mods out of firmware for now.
4. Do not mix firmware HRMs and host-side HRMs at the same time.
5. Use VIA for simple remaps only after the firmware structure is stable.
6. Keep advanced behavior such as combos and layer logic in source control here.

## Build and Test Checklist

After a firmware change, test these in order:

1. Base layer typing
2. `Nav/Caps Lock` thumb key
3. `Num + System/Tab` thumb key
4. `Nav` layer movement, search, and repeat flow
5. `Num + System` layer numbers, media, and `F1..F12`
6. `Keyboard` tri-layer
7. combos
8. RGB indicators
9. VIA detection

Also remember:

- `Shift + Backspace -> Delete` requires a real firmware Shift key
- `Shift + Escape -> ~` also requires a real firmware Shift key
- both-shifts can enable Caps Word

## Updating Upstream Later

When you want newer BastardKB/QMK changes:

1. update the vendor firmware repo
2. rebuild this userspace keymap against it
3. test layers, combos, RGB, and overrides
4. only then update docs or declare the new upstream state as good

Do not bury vendor changes inside this userspace repo. Keep the separation clear.

## CI

The included GitHub Actions workflow builds this userspace against:

- `bastardkb/bastardkb-qmk`
- branch `bkb-develop`

## Related Documentation

Detailed keymap behavior, layers, combos, and tuning notes live here:

- `keyboards/bastardkb/tbkmini/keymaps/reshka/readme.md`
