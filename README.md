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

Features
===

<h3>The editor will contain the following features.</h3>

<ol>
<li>It will read individual keypresses from the user and render the editor’s user interface to the screen after each keypress.</li>
<li>The user will be able to move the cursor around using arrow keys, Delete, Page-Up, Page-Down, Home and End.</li>
<li>The program will display text files, complete with vertical and horizontal scrolling and a status bar.</li>
<li>After editing the user can save the file to disk. A warning will be displayed for unsaved changes.</li>
<li>The program can be exited using Ctrl-Q.</li>
<li>It will support incremental search, meaning the file is searched after each keypress when the user is typing in their search query. The user can also cancel the search query. The user can advance to the next or previous match in the file using the arrow keys.</li>
<li>It will have syntax highlighting, search highlighting and filetype detection. It will highlight strings, digits, keywords, comments with different colors.</li>
</ol>
