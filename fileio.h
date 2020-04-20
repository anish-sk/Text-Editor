#ifndef FILEIO_H
#define FILEIO_H

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

#include"input.h"
#include"output.h"
#include"rowops.h"
#include"syntaxHighlight.h"

using namespace std;

/*** defines ***/

#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f) //Ctrl key combined with alphabetic keys map to the bytes 1-26

typedef std::basic_string<unsigned char> ustring;

/*** file i/o ***/

/* concatenating the rows of the file into a single string */
void editorRowsToString(string &buf);

/*opening and reading a file from disk*/
void editorOpen(char *filename);

/* saving the file */
void editorSave();

#endif