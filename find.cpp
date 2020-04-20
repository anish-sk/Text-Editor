#include"find.h"

/*** find ***/

/*finding a given string*/
void editorFindCallback(char *query, int key){
    static int last_match = -1;
    static int direction = 1; //variable containing direction to search

    static int saved_hl_line; //saving the line which is going to be highlighted
    static ustring saved_hl = ustring(); //saving the current highlight

    if(saved_hl.size()){
        E.hl[saved_hl_line]=saved_hl; //restoring the previous highlight
        saved_hl = ustring();
    }
    if(key == '\r' || key == '\x1b'){
        last_match = -1;
        direction = 1;
        return; //return from search when user presses enter or escape
    }
    else if(key == ARROW_RIGHT || key == ARROW_DOWN){
        direction = 1; //forward search
    }
    else if(key == ARROW_LEFT || key == ARROW_UP){
        direction = -1; //backward search
    }
    else {
        last_match = -1;
        direction = 1;
    }

    if(last_match == -1) direction = 1;
    int current = last_match;
    int i;
    for(int i=0;i<E.numrows;i++){
        current += direction; //calculating the row number to be searched for
        if(current == -1) current = E.numrows - 1; //wrap around from beginning to end
        else if(current == E.numrows) current = 0; //wrap around from end to beginning

        size_t match = E.render[current].find(query);
        if(match != string::npos){
            last_match = current;
            E.cy = current;
            E.cx =  editorRowRxtoCx(E.row[current],(int)match); //move the cursor to the match
            E.rowoff = E.numrows; //we scroll to the bottom of the file so that in the next refresh the match apears at top.
            
            saved_hl_line = current;
            saved_hl = E.hl[current]; //saving the current line before changing the highlight
            for(int i=0;i<strlen(query);i++){
                E.hl[current][match + i] = HL_MATCH;
            }
            break;
        }
    }
}

/*wrapper function for find in incremental search*/
void editorFind(){
    int saved_cx = E.cx; //save cursor position
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;

    char *query = editorPrompt("Search: %s (ESC/Arrows/Enter)", editorFindCallback); //get the string to be searched for from prompt

    if(query){
        free(query);
    }
    else{
        E.cx = saved_cx;//When the user pressed escape we restore the cursor to values before search
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
    }
}