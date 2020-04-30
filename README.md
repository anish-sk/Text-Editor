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

Learning Outcomes
===

<ol>
	<li>Generally I have dealt with only stdin and stdout, where the input is given
        after we press Enter and the output occurs sequentially.</li>
    <li> This project gave me 	an opportunity to look at the RAW mode of terminal, and explore how the
	terminal works using escape sequences extensively, for clearing screen,
        position cursor color characters, display in inverted color, etc.</li>
	<li>I also learnt how to share global variables across files using extern)
</ol>

Scope of improvement
===
<h3> Features that can be possibly added </h3>
    <ol>
        <li>Line Numbers display</li>
		<li>Auto indent</li>
		<li>Hard wrap lines: Insert a newline in the text when the user is
		about to type past the end of the screen, keeping in mind not to insert a newline where it would split a word</li>
		<li>Soft wrap lines: When a line is longer than the screen width,
		use multiple lines on the screen to display it instead of horizontal scrolling</li>
		<li>Use ncurses library for taking care of low level terminal
		handling.</li>
		<li>Copy and paste</li>
		<li>Config file to set TAB_STOP and Quit times</li>
		<li>Modal editing : like vim, normal, insert, visual, select, command </li>
		<li>Multiple Buffers</li>
    </ol>
