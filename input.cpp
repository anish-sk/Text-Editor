#include"input.h"
#include"find.h"

/*** input ***/

/* prompting the user to input a filename when saving a new file*/
char *editorPrompt(const char *prompt, void(*callback)(char *, int)){
    size_t bufsize = 128;
    char *buf = (char *)malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while(1){
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();
        
        int c = editorReadKey();
        if(c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
            if(buflen!=0) buf[--buflen]='\0';
        }
        else if(c=='\x1b'){
            editorSetStatusMessage("");
            if(callback) callback(buf, c);
            free(buf);
            return NULL; //the input prompt is cancelled when user presses escape
        }
        else if(c=='\r'){
            if(buflen!=0){
                editorSetStatusMessage("");//when the user presses enter the status message is cleared and user's input is returned. 
                if(callback) callback(buf, c);
                return buf;
            }
        }
        else if(!iscntrl(c) && c < 128 ){//checking if input key is in range of char
            if(buflen == bufsize - 1){
                bufsize *= 2;
                buf = (char *)realloc(buf, bufsize); //when we reach maximum capacity we reallocate memory
            }
            buf[buflen++]=c;
            buf[buflen]='\0';
        }
        if(callback) callback(buf, c);
    }
}

/* moves cursor according to key pressed */
void editorMoveCursor(int key){
    string row = (E.cy >= E.numrows) ? "" : E.row[E.cy]; 
    switch(key){
        case ARROW_LEFT:
            if(E.cx != 0){
                E.cx--;
            }            
            else if(E.cy > 0){
                E.cy--;
                E.cx = E.row[E.cy].size(); // Allowing the user to press the left arrow key at the beginning of the line to move to the end of the previous line. 
            }
            break;
        case ARROW_RIGHT:
            if(row!="" && (E.cx < (int)row.size())){ //preventing cursor to move past the line end
                E.cx++;
            }
            else if(row!="" && E.cx == (int)row.size()){
                E.cy++;
                E.cx = 0; // Allowing the user to press the right key at the end of a line to go to the beginning of the next line.
            }
            break;
        case ARROW_UP:
            if(E.cy != 0){
                E.cy--;
            }            
            break;
        case ARROW_DOWN:
            if(E.cy < E.numrows){
                E.cy++;
            }            
            break;
    }
    row = (E.cy >= E.numrows) ? "" : E.row[E.cy];
    if(E.cx > row.size()){
        E.cx = row.size(); //snapping cursor to end of line
    }
}

/* handling a keypress */

void editorProcessKeypress(){
    static int quit_times = EDITOR_QUIT_TIMES;

    int c = editorReadKey();

    switch(c){
        case '\r':
            editorInsertNewline();
            break;

        case CTRL_KEY('q'):
            if(E.dirty && quit_times > 0){
                editorSetStatusMessage("WARNING!!! File has unsaved changes. Press Ctrl-Q %d more times to quit.", quit_times);
                //warning the user of unsaved changes before quitting the editor
                quit_times -- ;
                return;
            }
            write(STDOUT_FILENO, "\x1b[2J", 4); //clears the screen
            write(STDOUT_FILENO, "\x1b[H",3); //repositions the cursor to the top left of the screen 
            exit(0); //quit when you read 'Ctrl-Q'
            break;

        case CTRL_KEY('s'):
            editorSave(); //Ctrl-S for save
            break;

        case HOME_KEY:
            E.cx = 0;
            break;

        case END_KEY:
            if(E.cy < E.numrows)
                E.cx = E.row[E.cy].size();
            break;

        case CTRL_KEY('f'):
            editorFind();
            break;

        case BACKSPACE:
        case CTRL_KEY('h'):
        case DEL_KEY:
            if(c == DEL_KEY) editorMoveCursor(ARROW_RIGHT);
            editorDelChar();
            break;

        case PAGE_UP:
        case PAGE_DOWN:
            {
                if(c == PAGE_UP){
                    E.cy = E.rowoff;
                }
                else if(c == PAGE_DOWN){
                    E.cy = E.rowoff + E.screenrows - 1;
                    if(E.cy > E.numrows) E.cy = E.numrows;
                }
                int times = E.screenrows;
                while(times--){
                    editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
                }
            }
            break;

        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            editorMoveCursor(c);
            break;
        
        case CTRL_KEY('l'):
        case '\x1b':
            break;
        
        default:
            editorInsertChar(c);
            break;
    }
    quit_times = EDITOR_QUIT_TIMES;
}