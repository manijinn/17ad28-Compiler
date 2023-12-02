

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "utilities.h"
#include "lexer.h"

FILE * myFile;
const char * filename;

token lex;

int column = 1;
int line = 1;

// Open the file, if file is not found, call lexical_error
extern void lexer_open(const char *fname)
{
    filename = fname;
    myFile = fopen(fname, "r");

    if(myFile == NULL)
    {
        lexical_error(fname, line, column, "file not found");
        exit(1);
    }
}

// Close file
extern void lexer_close()
{
    fclose(myFile);
}

// Check if lexer is done, which we will know when getc returns EOF
extern bool lexer_done()
{
    int ch = getc(myFile);

    if(ch != EOF && myFile != NULL)
    {
        ungetc(ch, myFile);
        return false;
    }

    return true;
}

// Iterate through the file
extern token lexer_next()
{
    int c;

    // We will use this to denote a segment of the code as a comment when needed
    bool isComment = false;

    // Get the first char that we'll process
    c = fgetc(myFile);

    lex.filename = lexer_filename();
    lex.column = column; 
    lex.line = line; 

    // This case scans for a # symbol, indicating a comment.
    if(c == '#') {

        // Since we found a comment, we make isComment true.
        isComment = true;
        
        while(isComment) {

        // Iterate through line until we reach the \n newline character
           while((c = fgetc(myFile)) != '\n') {
               
               continue;
            }

            // Get first character from next line
            c = fgetc(myFile);

            // If the next char is a normal letter, number, or punctuation that isn't #, comment ends
            if ((isalnum(c) || ispunct(c)) && c != '#') {
                
                isComment = false;
            }
        }
    }
  
    // Skip all whitespace characters
    while(isspace(c))
    {
        if(c == ' ')
        {
            column++;
        }
        else if(c == '\n')
        {
            column = 1;
            line++;
        }

        c = fgetc(myFile);
    }

    // Case that the char is a letter
    if(isalpha(c))
    {
        char string[255] = {};
        int i = 0;

        // Put char into string, increment i for next array space
        string[i] = c;
        i++;


        // Increment column
        column++;

        // Get next char
        c = fgetc(myFile);

        // Cycle through entire string, until we reach a non alpha-num char (like a whitespace)
        while(isalnum(c))
        {
            string[i] = c;
            i++;
            string[i] = '\0';
            
            c = fgetc(myFile);
            column++;

        }

        // If the last char stored was a punctuation, we unget it
        if (ispunct(c)) {
            
            ungetc(c, myFile);
        }

        // Allocate .text field of struct and store string into it
        lex.text = malloc(sizeof(char) * (strlen(string) + 1));
        strcpy(lex.text, string);

        // Depending on what lex.text stores, one of these cases will trigger, and appropriate values
        // will be assigned
        if(strcmp(lex.text, "const") == 0)
        {
            lex.typ = 1;
            lex.value = 1;
        }
        else if(strcmp(lex.text, "var") == 0)
        {
            lex.typ = 4;
            lex.value = 4;
        }
        else if(strcmp(lex.text, "procedure") == 0)
        {
            lex.typ = 5;
            lex.value = 5;
        }
        else if(strcmp(lex.text, "call") == 0)
        {
            lex.typ = 7;
            lex.value = 7;
        }
        else if(strcmp(lex.text, "begin") == 0)
        {
            lex.typ = 8;
            lex.value = 8;
        }
        else if(strcmp(lex.text, "end") == 0)
        {
            lex.typ = 9;
            lex.value = 9;
        }
        else if(strcmp(lex.text, "if") == 0)
        {
            lex.typ = 10;
            lex.value = 10;
        }
        else if(strcmp(lex.text, "then") == 0)
        {
            lex.typ = 11;
            lex.value = 11;
        }
        else if(strcmp(lex.text, "else") == 0)
        {
            lex.typ = 12;
            lex.value = 12;
        }
        else if(strcmp(lex.text, "while") == 0)
        {
            lex.typ = 13;
            lex.value = 13;
        }
        else if(strcmp(lex.text, "do") == 0)
        {
            lex.typ = 14;
            lex.value = 14;
        }
        else if(strcmp(lex.text, "read") == 0)
        {
            lex.typ = 15;
            lex.value = 15;
        }
        else if(strcmp(lex.text, "write") == 0)
        {
            lex.typ = 16;
            lex.value = 16;
        }
        else if(strcmp(lex.text, "skip") == 0)
        {
            lex.typ = 17;
            lex.value = 17;
        }
        else if(strcmp(lex.text, "odd") == 0)
        {
            lex.typ = 18;
            lex.value = 18;
        }
        else
        {
            lex.typ = 21;
            lex.value = 21;
        }

    }

    // If the char is a number
    else if(isdigit(c))
    {
        char number[255] = {};
        int i = 0;

        number[i] = c;
        i++;

        // Keep cycling through the number until a non-number input is encountered.
        for(;;)
        {
            c = fgetc(myFile);
            column++;

            if(isdigit(c))
            {
                number[i] = c;
                i++;
            }
            else
            {
                ungetc(c, myFile);
                column--;
                number[i] = '\0';
                break;
            }
        }
        
        int num = atoi(number); 

        // Check if number is between the short int min and max, otherwise it is too big
        if(SHRT_MIN < num && SHRT_MAX > num)  
        {
            lex.text = malloc(sizeof(char) * (strlen(number) + 1));
            strcpy(lex.text, number);
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 22;
            lex.value = atoi(number);
        }
        else
        {
            lexical_error(filename, line, column, "The value of %d is too large for a short!", num);
            exit(1);
        }
    }

    // Cases for simpler inputs (like puncution) down below
    else if(c == '.')
    {
        //printf("HIT");
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, ".");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 0;
        lex.value = 0;
    }
    else if(c == ';')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, ";");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 2;
        lex.value = 2;
    }  
    else if(c == ',')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, ",");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 3;
        lex.value = 3;
    }
    else if(c == ':')
    {
        lex.column = lexer_column();

        if((c = getc(myFile)) == '=')
        {
            column++;
            lex.text = malloc(sizeof(char) * 3);
            strcpy(lex.text, ":=");
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 6;
            lex.value = 6;
        }
        else
        {
            c = ungetc(c, myFile);
            column--;
        }
    } 
    else if(c == '(')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, "(");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 19;
        lex.value = 19;
    }
    else if(c == ')')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, ")");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 20;
        lex.value = 20;
    }
    else if(c == '=')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, "=");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 23;
        lex.value = 23;
    }

    // If < char found, compare with sub-cases
    else if(c == '<')
    {
        lex.column = lexer_column();

        c = fgetc(myFile);
        if(c == '>')
        {
            column++;
            lex.text = malloc(sizeof(char) * 3);
            strcpy(lex.text, "<>");
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 24;
            lex.value = 24;
        }
        else if(c == ' ')
        {    
            ungetc(c, myFile);
            lex.text = malloc(sizeof(char) * 2);
            strcpy(lex.text, "<");
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 25;
            lex.value = 25;
        }
        else if(c == '=')
        {
            column++;
            lex.text = malloc(sizeof(char) * 3);
            strcpy(lex.text, "<=");
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 26;
            lex.value = 26;
        }
        else
        {
            c = ungetc(c, myFile);
            column--;
        }
    }

    // If > char found, compare with sub-cases
    else if(c == '>')
    {
        lex.column = lexer_column();
        c = fgetc(myFile);
        if(c == '<')
        {
            column++;
            lex.text = malloc(sizeof(char) * 3);
            strcpy(lex.text, "><");
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 24;
            lex.value = 24;
        }
        else if(c == ' ')
        {    
            ungetc(c, myFile);
            lex.text = malloc(sizeof(char) * 2);
            strcpy(lex.text, ">");
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 25;
            lex.value = 25;
        }
        else if(c == '=')
        {
            column++;
            lex.text = malloc(sizeof(char) * 3);
            strcpy(lex.text, ">=");
            lex.text[strlen(lex.text)] = '\0';
            lex.typ = 26;
            lex.value = 26;
        }
        else
        {
            c = ungetc(c, myFile);
            column--;
        }
    }
 
    else if(c == '+')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, "+");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 29;
        lex.value = 29;
    }
    else if(c == '-')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, "-");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 30;
        lex.value = 30;
        
    }
    else if(c == '*')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, "*");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 31;
        lex.value = 31;
    }
    else if(c == '/')
    {
        lex.text = malloc(sizeof(char) * 2);
        strcpy(lex.text, "/");
        lex.text[strlen(lex.text)] = '\0';
        lex.typ = 32;
        lex.value = 32;
    }
    // Check for space again
    else if(isspace(c))
    {
        if(c == ' ')
        {
            column++;
        }
        else if(c == '\n')
        {
            column = 1;
            line++;
        }
        else
        {
            exit(1);
        }
    }

    // End of file case
    else if((c = getc(myFile)) == EOF)
    {
        line++;
        column = 1;
        lex.typ = 33;
        lex.value = 33;
        lex.column = lexer_column();
        lex.line = lexer_line();
       
        lex.text = NULL;

    }
    // If none of the above cases apply, there is clearly something wrong, like an illegal character
    else
    {
        lexical_error(lex.filename, line, column, "llegal character '%c' (%o)", c, c);
        exit(1);
      
    }

    // Return filled token
    return lex;
}

// Getter functions
extern const char *lexer_filename()
{
    return filename;
}

extern unsigned int lexer_line()
{
    return line;
}

extern unsigned int lexer_column()
{
    return column;
}