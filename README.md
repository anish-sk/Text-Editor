Textor
===

Textor is a small text editor.

```console
Usage:
    usr:~$ make
    usr:~$ ./textor `<filename>` 
```
Leave the filename blank to open a new blank file which can be saved later.
```console
Keys:
    CTRL-S: Save
    CTRL-Q: Quit
    CTRL-F: Find string in file (ESC to exit search, arrows to navigate)
```
Textor also provides syntax highlighting for C/C++ type files, and can  easily be
extended to other filetypes by adding a suitable entry to the highlight database
vector HLDB in the file main.cpp.

Textor does not depend on any library (not even curses). It uses fairly standard
VT100 (and similar terminals) escape sequences.
