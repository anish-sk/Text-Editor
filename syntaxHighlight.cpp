#include"syntaxHighlight.h"

/*** syntax highlighting ***/

/*returns whethere a character is a separator character*/
int is_separator(int c){
    return isspace(c) || c =='\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

/*updating the hl array for highlighting*/
ustring editorUpdateSyntax(string render, int idx){
    ustring h = ustring(render.size(),HL_NORMAL);
    if(E.syntax == NULL) return h;

    vector<string> keywords = E.syntax->keywords;

    string scs = E.syntax->singleline_comment_start;//start of single line comment
    string mcs = E.syntax->multiline_comment_start;//start of multi line comment
    string mce = E.syntax->multiline_comment_end;//end of multi line comment

    int scs_len = scs.size(); 
    int mcs_len = mcs.size();
    int mce_len = mce.size();

    int prev_sep = 1; //keeps track of whether the previous character was a separator
    int in_string = 0; //keeps track of whether we are inside a string or not and also stores the beginning quote
    int in_comment =(idx > 0 && E.hl_open_comment[idx-1]); //keeps track of whether we are inside a multi line comment or not

    int i=0;
    while(i<render.size()){
        char c = render[i];
        unsigned char prev_hl = (i>0) ? h[i-1]: HL_NORMAL; //highlight type of the previous character

        if(scs_len && !in_string && !in_comment){
            if(render.substr(i,scs_len)==scs){//if the current charcter is the start of a single line comment
                for(int j=i;j<=render.size();j++){
                    h[j]=HL_COMMENT; //we set the rest of the line with HL_COMMENT
                }
                return h;
            }
        }

        if(mcs_len && mce_len && !in_string){
            if(in_comment){
                h[i] = HL_MLCOMMENT;
                if(render.substr(i,mce_len)==mce){
                    for(int j=i;j<i+mce_len;j++){//if the current character is the end of a multi line comment
                        h[j]=HL_MLCOMMENT;//we highlight the end string and reset the flag
                    }
                    i+= mce_len;
                    in_comment= 0;
                    prev_sep = 1;
                    continue;
                }
                else{
                    i++;
                    continue;
                }
            }
            else if(render.substr(i,mcs_len)==mcs){//if the current character is the start of a multi line comment
                    for(int j=i;j<i+mcs_len;j++){
                        h[j]=HL_MLCOMMENT;//we highlight the start string and set the flag
                    }
                    i+= mcs_len;
                    in_comment= 1;
                    continue;
            }
        }

        if(E.syntax->flags & HL_HIGHLIGHT_STRINGS){//highlight strings enabled
            if(in_string){
                h[i] = HL_STRING;
                if(c=='\\' && i+1<render.size()){
                    h[i+1]=HL_STRING;//taking escaped quotes into consideration
                    i+=2;
                    continue;
                }
                if(c == in_string) in_string=0; //we encounter the end quote, then we reset in_string
                i++;
                prev_sep = 1;
                continue;
            }
            else{
                if(c=='"' || c=='\''){
                    in_string = c; //we set in_string when we encounter a beginning quote
                    h[i]=HL_STRING;
                    i++;
                    continue;
                }
            }
        }

        if(E.syntax->flags & HL_HIGHLIGHT_NUMBERS){ //highlight numbers enabled
            if((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c=='.' && prev_hl == HL_NUMBER)){
                //a digit is highlighted if either previous character is a separator or is also highlighted with HL_NUMBER or a decimal point 
                h[i] = HL_NUMBER; //for digits hl value is HL_NUMBER
                i++;
                prev_sep = 0;
                continue;
            }
        }

        if(prev_sep){
            int j;
            for(j=0; j<keywords.size(); j++){
                int klen = keywords[j].size();
                bool kw2 = keywords[j][klen-1] == '|'; //check for datatype
                string key = keywords[j];
                int hi = kw2?HL_KEYWORD2:HL_KEYWORD1;//highlight to be set
                if(kw2) {klen--;key.pop_back();}

                if((render.substr(i,klen)== key)&&((i+klen == render.size())|| is_separator(render[i+klen]))){ //the current character is the start of a keyword
                    for(int j=i;j<i+klen;j++){
                        h[j]=hi;
                    }
                    i+=klen;
                    break;
                }
            }
            if(j!=keywords.size()){
                prev_sep = 0;
                continue; 
            }

        }

        prev_sep = is_separator(c);
        i++;
    }
    int changed = (E.hl_open_comment[idx]!=in_comment);
    E.hl_open_comment[idx] = in_comment;
    if(changed && ((idx + 1) < E.numrows))
    E.hl[idx+1] = editorUpdateSyntax(E.render[idx+1],idx+1); //if the current multi line comment is not closed we change the highlight of the next line as well
    return h;
}

/*gives the color for the required syntax type*/
int editorSyntaxToColor(int hl){
    switch(hl){
        case HL_COMMENT:
        case HL_MLCOMMENT: return 36; //cyan for comments
        case HL_KEYWORD1: return 33; //yellow for actual keywords
        case HL_KEYWORD2: return 32; //green for common type names
        case HL_STRING: return 35; //magenta for strings
        case HL_NUMBER: return 31; //red for numbers
        case HL_MATCH: return 34; //blue for successful match in a search
        default: return 37;
    }
}

/*matching the current filename to one of the filematch fields in HLDB*/
void editorSelectSyntaxHighlight(){
    E.syntax = NULL;
    if(E.filename == "") return;

    int pos = (E.filename.find_last_of('.')); //last occurrence of . in filename
    string ext = ""; 
    if(pos != string ::npos) ext = E.filename.substr(pos); //extension of file

    for(unsigned int j=0; j< HLDB_ENTRIES; j++){
        editorSyntax *s = &HLDB[j];
        unsigned int i = 0;
        while(i<s->filematch.size() && s->filematch[i] != ""){//iterate throgh strings in filematch
            int is_ext = (s->filematch[i][0] == '.');
            if((is_ext && ext.size() && (ext == s->filematch[i]))|| (!is_ext && (E.filename.find(s->filematch[i])!=string::npos))){
                E.syntax = s;//if pattern is an extension then we see whether the filename ends with that extension, otherwise we check to see whether the pattern exists anywherre in the filename
                int filerow;
                for(filerow=0;filerow<E.numrows;filerow++){
                    E.hl[filerow]=editorUpdateSyntax(E.render[filerow],filerow);//rehighlighting after the filetype changes
                }
                return; 
            }
            i++;
        }
    }


}