# Geometer

Vim-like geometry box for fast figure creation with LaTeX!

This grew out of a combination of frustration at Inkscape for not having a simple way to
do things (or maybe I'm just bad at learning things) for mathematical figure generation,
and that tools like Geometer's Sketchpad don't have good FOSS equivalents. Although
this doesn't try to replicate completely either feature-sets, it is pretty usable,
and planned features should help with making it more so.

## Features

- [x] Creating lines, circles, and text
- [x] Creating bezier curves
- [x] Creating arcs
- [x] Deleting elements
- [ ] Editing elements (need to implement a node system)
- [ ] Persistence
- [ ] Saving to a TeX file
- [ ] Having a relatively complete command language

## Usage

I'd advise you to look at `execute.c` which contains, in `execute`, the
commands that are executed in command mode (after you press `:`), and in
`shortcut`, the commands that are executed in normal mode.

## Architecture (for developers = me)

It's really cursed, but trust me, it's fine!

### Global variables

- `buffer`: the text that is rendered in the bottom left side of the screen (or not?). It is 256 chars big.
- `cursor`: the position of the cursor in the buffer.
- `mode`: the OperationMode (duh)
- `locked`: whether or not the cursor is locked (affects the color of the cursor)
- `ip`: a list of all the points that have been clicked so far; used in the construction of the final `UserObject`
- `objects`: a list of all `UserObjects` that are complete and need to be rendered

Constants:
- `types`: This is very important! This is where all the logic for drawing/persisting each type is stored: an array
    containing strings, info, and function pointers (object-oriented programming in C?? Sacrilege!). It's in the
    middle of `render_info.c` for some reason... I couldn't tell you why it's there.

### Types

- `OperationMode`: used for the `mode` variable
- `UserObject`: used for rendered objects. This type is really weird: it consists of a type, which starts from 100, and
    then an anonymous struct. This is done to allow it to be indexed as an array to reduce code bloat, but it means that
    the layout of this struct is important to the functioning of the program. There is also text, for text objects,
    and an unused (for now) color attribute.
- `UserObjectSeq` and `UserObjectList`: book-keeping structs
