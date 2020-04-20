#include"editorConfig.h"
#include"rowops.h"

/*** row operations ***/

/*convert row index to render index*/
int editorRowCxtoRx(string &row, int cx){
    int rx = 0;
    for(int j=0; j<cx; j++){
        if(row[j] == '\t')
            rx += (EDITOR_TAB_STOP - 1) - (rx % EDITOR_TAB_STOP); //advance to the next tab stop
        rx++;
    }
    return rx;
}

/*convert render index to row index (essential inversion of the function editorRowCxtoRx*/
int editorRowRxtoCx(string &row, int rx){
    int cur_rx = 0;
    int cx;
    for(cx=0; cx<row.size(); cx++){
        if(row[cx]=='\t')
            cur_rx+= (EDITOR_TAB_STOP - 1) - (cur_rx % EDITOR_TAB_STOP);
        cur_rx++;

        if(cur_rx > rx) return cx;
    }
    return cx;
}

/*fill the render string using row*/
string editorUpdateRow(string row){
    string render="";
    int idx=0;
    for(auto i: row){
        if(i=='\t'){
            render+=' ';idx++;
            while(idx % EDITOR_TAB_STOP != 0){//we advance till the next tab stop which is a column divisible by the tab_stop
                render+=' ';idx++;
            }
        }
        else{
            render+=i; idx++;
        }
    }
    return render;
}

/*insert a row into the editor*/
void editorInsertRow(int at, string s, int len){
    if(at == E.row.size()){
        E.row.push_back(s.substr(0,len)); 
        E.render.push_back(editorUpdateRow(s.substr(0,len)));
        E.hl_open_comment.push_back(0);
        E.hl.push_back(editorUpdateSyntax(E.render.back(),E.render.size()-1));
    }
    else{
        E.row.insert(E.row.begin()+at,s.substr(0,len));
        string r = editorUpdateRow(s.substr(0,len)); 
        E.render.insert(E.render.begin()+at,r);
        E.hl_open_comment.insert(E.hl_open_comment.begin()+at,0);
        E.hl.insert(E.hl.begin()+at,editorUpdateSyntax(r,at));
    }
    E.numrows++;
    E.dirty++;
}

/* deleting a row */
void editorDelRow(int at){
    if(at < 0 || at >= E.numrows) return;
    E.row.erase(E.row.begin()+at);
    E.render.erase(E.render.begin()+at);
    E.hl_open_comment.erase(E.hl_open_comment.begin()+at);
    E.numrows--;
    E.dirty++;
}

/* insert a character into row*/
void editorRowInsertChar(string &row, int at, char c, int pos){
    if(at < 0 || at > row.size()){
        at = row.size();
    }
    row.insert(row.begin()+at,c);
    E.render[pos] = editorUpdateRow(row);
    E.hl[pos] = editorUpdateSyntax(E.render[pos],pos);
    E.dirty++;
}

/* Appending one row to another */
void editorRowAppendString(string &row, string to_append, int pos){
    row+=to_append;
    E.render[pos] = editorUpdateRow(row);
    E.hl[pos] = editorUpdateSyntax(E.render[pos], pos);
    E.dirty++;
}

/* delete a character from row*/
void editorRowDelChar(string &row, int at, int pos){
    if(at < 0 || at >= row.size()) return;
    row.erase(row.begin()+at);
    E.render[pos] = editorUpdateRow(row);
    E.hl[pos] = editorUpdateSyntax(E.render[pos],pos);
    E.dirty++;
}

/*** editor operations***/

/* insert a character in the current position */
void editorInsertChar(char c){
    if(E.cy == E.numrows){
        editorInsertRow(E.numrows,"",0); //If the cursor is at the end we insert an empty row
    }
    editorRowInsertChar(E.row[E.cy], E.cx, c, E.cy);
    E.cx++;
}

/* insert newline when Enter key is pressed*/
void editorInsertNewline(){
    if(E.cx == 0){
        editorInsertRow(E.cy,"",0);//If we are at the beginnig of a line , we just need to insert a blank row above
    }
    else{
        string row = E.row[E.cy];
        editorInsertRow(E.cy+1,row.substr(E.cx),row.size()-E.cx); //We insert the characters that are to the right of the cursor
        E.row[E.cy].erase(E.cx); //We truncate the current row;
        row = E.row[E.cy];
        E.render[E.cy]= editorUpdateRow(row);
        E.hl[E.cy]=editorUpdateSyntax(E.render[E.cy],E.cy);
    }
    E.cy++;
    E.cx=0;
}

/*deletes a character from the current position */
void editorDelChar(){
    if(E.cy == E.numrows) return;
    if(E.cx == 0 && E.cy == 0) return;
    if(E.cx > 0){
        editorRowDelChar(E.row[E.cy], E.cx-1, E.cy);
        E.cx--;
    }else{
        E.cx = E.row[E.cy-1].size(); //Pressing backspace from the beginning of a line.
        editorRowAppendString(E.row[E.cy-1], E.row[E.cy],E.cy-1); //We append the current row too the previous row
        editorDelRow(E.cy); //The current row is deleted
        E.cy--;
    }
}