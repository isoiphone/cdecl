// cdecl, decode and explain c declarations, no matter how obfuscated
// based on sample algorithm/code given on page 84-91 of Expert C Programming: Deep C Secrets, Peter Van Der Linden
// hacked together September 2013
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXTOKENLEN 64
#define MAXTOKENS   256

#define TYPE        'T'
#define QUALIFIER   'Q'
#define IDENTIFIER  'I'

struct token {
    char type;
    char string[MAXTOKENLEN];
};

//
// data
int top=-1;
struct token stack[MAXTOKENS];
struct token this;

//
// utility routines

#define push(T) stack[++top] = T
#define pop() stack[top--]
#define peek() stack[top]
#define empty() (top==-1)

void classify_string()
{
    const char* string = this.string;
    if (!strcmp(string, "void")
        || !strcmp(string, "char")
        || !strcmp(string, "short")
        || !strcmp(string, "int")
        || !strcmp(string, "long")
        || !strcmp(string, "signed")
        || !strcmp(string, "unsigned")
        || !strcmp(string, "double")
        || !strcmp(string, "struct")
        || !strcmp(string, "enum")
        || !strcmp(string, "union") ) {
        this.type = TYPE;
    } else if (!strcmp(string, "const")) {
        strcpy(this.string, "read-only");
        this.type = QUALIFIER;
    } else if (!strcmp(string, "volatile")) {
        this.type = QUALIFIER;
    } else {
        this.type = IDENTIFIER;
    }
}

void gettoken()
{
    char* str = this.string;
    
    while ( isspace(*str = getchar()) ) {}
    
    if (isalnum(*str)) {
        while (isalnum(*++str = getchar())) {}
        ungetc(*str, stdin);
        *str = '\0';
        classify_string();
    } else {
        this.type = *str;
        if (this.type == '*') {
            strcpy(this.string, "pointer to");
        } else {
            *++str = '\0';
        }
    }
}

void read_to_first_identifier()
{
    while (1) {
        gettoken();
        if (this.type == IDENTIFIER) {
            break;
        }
        push(this);
    }
    
    printf("%s is ", this.string);
    
    gettoken();
}

//
// parsing routines

void deal_with_function_args()
{
    if (this.type == '(') {
        do {
            gettoken();
        } while (this.type != ')');
        gettoken();
        printf("function returning ");
    }
}

void deal_with_arrays()
{
    while (this.type == '[') {
        printf("array ");
        gettoken();
        if (isdigit(this.string[0])) {
            const int size = atoi(this.string);
            printf("[0..%d] ", size-1);
            gettoken();
        }
        gettoken();
        printf("of ");
    }
}

void deal_with_pointers()
{
    while (peek().type == '*') {
        printf("pointer to ");
        pop();
    }
}

void deal_with_declarator()
{
    deal_with_arrays();
    deal_with_function_args();
    deal_with_pointers();

    while (!empty()) {
        if (peek().type == '(') {
            pop();
            gettoken();
            deal_with_declarator();
        } else {
            printf("%s ", pop().string);
        }
    }
}

//
// main

int main(int argc, const char * argv[])
{
    // use a file as STDIN, for testing inside of xcode
    freopen("input.txt", "r", stdin);
    
    read_to_first_identifier();
    deal_with_declarator();
    printf("\n");
    return 0;
}
