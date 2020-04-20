#ifndef ROW_H 
#define ROW_H

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
#include"syntaxHighlight.h"

/*** defines ***/

#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

typedef std::basic_string<unsigned char> ustring;


/*** row operations ***/

/*convert row index to render index*/
int editorRowCxtoRx(string &row, int cx);

/*convert render index to row index (essential inversion of the function editorRowCxtoRx*/
int editorRowRxtoCx(string &row, int rx);

/*fill the render string using row*/
string editorUpdateRow(string row);

/*insert a row into the editor*/
void editorInsertRow(int at, string s, int len);

/* deleting a row */
void editorDelRow(int at);

/* insert a character into row*/
void editorRowInsertChar(string &row, int at, char c, int pos);

/* Appending one row to another */
void editorRowAppendString(string &row, string to_append, int pos);

/* delete a character from row*/
void editorRowDelChar(string &row, int at, int pos);

/*** editor operations***/

/* insert a character in the current position */
void editorInsertChar(char c);

/* insert newline when Enter key is pressed*/
void editorInsertNewline();

/*deletes a character from the current position */
void editorDelChar();

#endif