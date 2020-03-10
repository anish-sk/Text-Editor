/*** includes ***/
#include<bits/stdc++.h>
#include<ctype.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>

using namespace std;

/*** data ***/
termios orig_termios;

/*** terminal ***/
/*printing error message and exiting the program*/
void die(const char *s){
    perror(s);
    exit(1);
}

/*disabling raw mode*/
void disableRawMode(){
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &orig_termios)==-1)
    die("tcsetattr");
}

/*enabling raw mode*/
void enableRawMode(){    
    if(tcgetattr(STDIN_FILENO, &orig_termios)==-1) die("tcgetattr"); //reading the current attributes into a global class instance
    atexit(disableRawMode); //we register our disableRawMode() function to be called automatically when the program exits.

    termios raw = orig_termios; // we assign the original instance to another instance to make changes
    /*(IXON)Turning Ctrl-S and Ctrl-Q flags off
    Fixing Ctrl-M
    turning off some miscellaneous flags which are generally turned off by default*/
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP );
    /*preventing translation of "\n" to "\r\n"*/
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    /*setting ECHO (which causes each key we type to be printed to the terminal) feature, 
    ICANON(which turns on canonical mode where we read line by line) feature, 
    ISIG(which turns off SIGINT(Ctrl-C) and SIGTSTP(Ctrl-Z)) to false
    IEXTEN(which turns of Ctrl-V)*/
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0; //setting the minimum number of bytes of inout needed before read() can return
    raw.c_cc[VTIME] = 1; //setting the maximum amount of time to wait before read() returns

    if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw)==-1) die("tcsetattr"); //passing the modified instance to wrtie the new terminal attributes back out
}

/*** init ***/
int main(){
    enableRawMode();

    
    while(1){
        char c='\0';
        if(read(STDIN_FILENO,&c,1)==-1 && errno!= EAGAIN) die("read");
        if(iscntrl(c)){
            printf("%d\r\n",c); //If 'c' is a nonprintable character, we print only the ASCII value
        }
        else{
            printf("%d ('%c')\r\n",c,c); // If 'c' is printable we print the ASCII value as well as the character it represents
        }
        if(c=='q') break; //quit when you read 'q'
    } 
    return 0;
}