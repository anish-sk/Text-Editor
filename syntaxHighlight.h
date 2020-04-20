#ifndef E_SYNTAX_HIGHLIGHT_H
#define E_SYNTAX_HIGHLIGHT_H

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

using namespace std;

/*** defines ***/

#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

#define HL_HIGHLIGHT_NUMBERS (1<<0) //flag for enabling the numbers highlighting
#define HL_HIGHLIGHT_STRINGS (1<<1) //flag for enabling the strings highlighting

typedef std::basic_string<unsigned char> ustring;

/*** syntax highlighting ***/

/*returns whethere a character is a separator character*/
int is_separator(int c);

/*updating the hl array for highlighting*/
ustring editorUpdateSyntax(string render, int idx);

/*gives the color for the required syntax type*/
int editorSyntaxToColor(int hl);

/*matching the current filename to one of the filematch fields in HLDB*/
void editorSelectSyntaxHighlight();

#endif