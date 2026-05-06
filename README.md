# QMK Userspace

This is a local QMK userspace for a TBK Mini on Splinky v3.

## Firmware base

- firmware repo: `bastardkb-qmk` on `bkb-master`
- userspace repo: this repository
- custom keymap: `keyboards/bastardkb/tbkmini/keymaps/reshka`

## Local setup

Point QMK at the BastardKB firmware tree and this external userspace:

```sh
qmk config user.qmk_home="/Users/0xse.reshka/Projects/Other/bastardkb-qmk"
qmk config user.overlay_dir="/Users/0xse.reshka/Projects/Other/bastardkb-qmk-userspace"
```

## Build

```sh
qmk compile -kb bastardkb/tbkmini -km reshka
```

## Documentation

Keymap-specific documentation lives here:

- `keyboards/bastardkb/tbkmini/keymaps/reshka/readme.md`
