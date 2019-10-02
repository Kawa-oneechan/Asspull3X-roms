# Asspull IIIx
## ROM source archive
This repository contains the full source for the *Asspull IIIx* ROM files — the BIOS and boot firmware, and a handful of functionality tests. They can be built in [devkitPro](https://devkitpro.org/).
### `_disk1_` and `_disk2_`
These two folders are the source for `disk1.img` and `disk2.img`, as built with the included copy of `bfi.exe`.
### `bios`
This is the complete source of the BIOS and boot firmware. It compiles to `ass-bios.apb` ("*a*ss*p*ull *b*ios") and contains several functions available to other programs, subdivided into a couple different libraries, Amiga style.
### `cmd`
This is an attempt to create a DOS prompt, basically, to navigate standard-issue 1.44 MB FAT floppy diskettes. It's very simple and does not currently work *at all*. Last time it did, it had `cd`, `dir`, `ren`, `del`, and `show` commands. The `show` command would work on `txt` files as well as images in `api` format.
### `devkit68k`
Contains the linker scripts and such. Merge it into devkitPro's `devkit68K` folder.
### `disktest`
A complete mess that was at one point a disk navigator.
### `fromdisk`
Hello World, but as a file on a bootable diskette. It compiles straight into `_disk2_` as an `app` file ("*a*ss*p*ull *p*rogram"). The BIOS can then detect its presence.
### `helloworld`
Much the same, but as a cart application. Was also a MIDI piano at one point.
### `lab`
This defies description. It has been many things, all buried in comment blocks.
### `ranmya`
A more graphical Hello World featuring the *previous*, placeholder mascot. Should be updated and renamed.
### `rawtest`
An old, short test involving interrupts that has to be compiled with the BIOS rules.
### `slideshow`
Tries to display all the `api` files on the currently-inserted diskette.
### `tiletest`
Early work to test the games console-style tilemap mode.
### `twohundred`
Normally, the A3X draws 240 or 480 lines, but by setting the bold font flag in graphics mode you can make it draw 200 or 400 lines, stretched.
### Several `png` and `api` files in the root
The `png` files can be passed to the included `img2ass` tool to convert them to `api` files ("*a*ss*p*ull *i*mage"). The various applications listed above can use them.
## Acknowledgements
* `bios` uses [*FatFS* by ChaN](http://elm-chan.org/fsw/ff/00index_e.html). The splash screen is edited from the copy protection screen from *Ranma ½ Hiryuu Densetsu*.
* *Farah Furcett* character named by Screwtape.
