#ifndef OUTPUT_H
#define OUTPUT_H

/*** includes ***/

#include<bits/stdc++.h>
#include<fstream>
#include<ctype.h>
#include<errno.h>
#include<fcntl.h>
#include<filesystem>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<termios.h>
#include<time.h>
#include<unistd.h>

#include"editorConfig.h"
#include"editorHighlight.h"
#include"rowops.h"

using namespace std;

/*** defines ***/

#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

typedef std::basic_string<unsigned char> ustring;

/*** output ***/

/*scrolling the editor*/
void editorScroll();

/* drawing ~ */
void editorDrawRows(string &ab);

/* drawing status bar at the bottom */
void editorDrawStatusBar(string &ab);

/* drawing message bar */
void editorDrawMessageBar(string &ab);

/* refreshing the screen */
void editorRefreshScreen();

/*set status message*/
void editorSetStatusMessage(const char *fmt, ...);

#endif