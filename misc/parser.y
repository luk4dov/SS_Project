%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    int yylex(void);
    int yyerror(const char* s);

    typedef unsigned int uint32;

    extern void selectInstruction(const char*, int, int, uint32, const char*, int);
    extern void selectDirective(const char*, const char*, uint32);
    extern void labelDefinition(const char*);
    extern void equDirective(uint32, const char*, const char*, const char*, uint32, uint32, uint32);

%}

%union {
    int num;
    short regn;
    char *str;
}

%token <num> NUMBER
%token <regn> REGISTER
%token <str> IDENTIFIER CSR SYMBOL_REF STRING_LITERAL

%token EXTERN GLOBAL SECTION WORD END ASCII SKIP EQU
%token LD ST PUSH POP XCHG
%token ADD SUB MUL DIV
%token NOT AND OR XOR
%token SHL SHR
%token JMP BEQ BNE BGT CALL RET IRET HALT INT
%token CSRRD CSRWR
%token OBRACKET CBRACKET PLUS MINUS
%token COMMA COLON NEWLINE DOLLAR QUOTE

%type <num> imm
%type <str> label
%type <str> string


%%

input:

|   input line

line:
    directive NEWLINE
|   label_definition NEWLINE
|   instruction NEWLINE
|   NEWLINE
|   error NEWLINE { yyerror("parsing error\n"); };

directive:
    GLOBAL global_identifier_list
|   EXTERN extern_identifier_list
|   SECTION IDENTIFIER { selectDirective("section", $2, 0); }
|   WORD word_identifier_list
|   SKIP NUMBER { selectDirective("skip", "", $2); }
|   ASCII string { selectDirective("ascii", $2, 0); }
|   EQU IDENTIFIER COMMA NUMBER { equDirective(0, $2, "", "", $4, -1, 0); }
|   EQU IDENTIFIER COMMA NUMBER PLUS NUMBER { equDirective(1, $2, "", "", $4, $6, 1); }
|   EQU IDENTIFIER COMMA NUMBER MINUS NUMBER { equDirective(1, $2, "", "", $4, $6, 2); }
|   EQU IDENTIFIER COMMA IDENTIFIER { equDirective(2, $2, $4, "", -1, -1, 0); }
|   EQU IDENTIFIER COMMA IDENTIFIER PLUS NUMBER { equDirective(3, $2, $4, "", $6, -1, 1); }
|   EQU IDENTIFIER COMMA IDENTIFIER MINUS NUMBER { equDirective(3, $2, $4, "", $6, -1, 2); }
|   EQU IDENTIFIER COMMA IDENTIFIER MINUS IDENTIFIER { equDirective(4, $2, $4, $6, -1, -1, 2); }
|   END { return 0; };


extern_identifier_list:
    IDENTIFIER { selectDirective("extern", $1, 0); }
|   extern_identifier_list COMMA IDENTIFIER { selectDirective("extern", $3, 0); }

global_identifier_list:
    IDENTIFIER { selectDirective("global", $1, 0); }
|   global_identifier_list COMMA IDENTIFIER { selectDirective("global", $3, 0); }

word_identifier_list:
    IDENTIFIER { selectDirective("word", $1, 0); }
|   NUMBER     { selectDirective("word", "", $1); }
|   word_identifier_list COMMA IDENTIFIER { selectDirective("word", $3, 0); }
|   word_identifier_list COMMA NUMBER     { selectDirective("word", "", $3); }


label_definition:
    label COLON;

label:
    IDENTIFIER { $$ = $1; labelDefinition($1); };

string:
    QUOTE IDENTIFIER QUOTE { $$ = $2; }
|   STRING_LITERAL { $$ = $1; };

imm:
    NUMBER { $$ = $1; };


instruction:
    HALT {
        selectInstruction("halt", 0, 0, 0, "", 0);
    }
|   INT {
        selectInstruction("int", 0, 0, 0, "", 0);
    }
|   IRET {
        selectInstruction("iret", 0, 0, 0, "", 0);
    }
|   CALL IDENTIFIER {
        selectInstruction("call", 0, 0, 0, $2, 0);
    }
|   CALL imm {
        selectInstruction("call", 0, 0, $2, "", 1);
    }
|   RET {
        selectInstruction("ret", 0, 0, 0, "", 0);
    }
|   JMP imm {
        selectInstruction("jmp", 0, 0, $2, "", 0);
    }
|   JMP IDENTIFIER {
        selectInstruction("jmp", 0, 0, 0, $2, 1);
    }
|   BEQ REGISTER COMMA REGISTER COMMA imm {
        selectInstruction("beq", $2, $4, $6, "", 0);
    }
|   BEQ REGISTER COMMA REGISTER COMMA IDENTIFIER {
        selectInstruction("beq", $2, $4, 0, $6, 1);
    }
|   BNE REGISTER COMMA REGISTER COMMA imm {
        selectInstruction("bne", $2, $4, $6, "", 0);
    }
|   BNE REGISTER COMMA REGISTER COMMA IDENTIFIER {
        selectInstruction("bne", $2, $4, 0, $6, 1);
    }
|   BGT REGISTER COMMA REGISTER COMMA imm {
        selectInstruction("bgt", $2, $4, $6, "", 0);
    }
|   BGT REGISTER COMMA REGISTER COMMA IDENTIFIER {
        selectInstruction("bgt", $2, $4, 0, $6, 1);
    }
|   PUSH REGISTER {
        selectInstruction("push", $2, 0, 0, "", 0);
    }
|   POP REGISTER {
        selectInstruction("pop", $2, 0, 0, "", 0);
    }
|   XCHG REGISTER COMMA REGISTER {
        selectInstruction("xchg", $2, $4, 0, "", 0);
    }
|   ADD REGISTER COMMA REGISTER {
        selectInstruction("add", $2, $4, 0, "", 0);
    }
|   SUB REGISTER COMMA REGISTER {
        selectInstruction("sub", $2, $4, 0, "", 0);
    }
|   MUL REGISTER COMMA REGISTER {
        selectInstruction("mul", $2, $4, 0, "", 0);
    }
|   DIV REGISTER COMMA REGISTER {
        selectInstruction("div", $2, $4, 0, "", 0);
    }
|   NOT REGISTER {
        selectInstruction("not", $2, 0, 0, "", 0);
    }
|   AND REGISTER COMMA REGISTER {
        selectInstruction("and", $2, $4, 0, "", 0);
    }
|   OR REGISTER COMMA REGISTER {
        selectInstruction("or", $2, $4, 0, "", 0);
    }
|   XOR REGISTER COMMA REGISTER {
        selectInstruction("xor", $2, $4, 0, "", 0);
    }
|   SHL REGISTER COMMA REGISTER {
        selectInstruction("shl", $2, $4, 0, "", 0);
    }
|   SHR REGISTER COMMA REGISTER {
        selectInstruction("shr", $2, $4, 0, "", 0);
    }
|   LD DOLLAR imm COMMA REGISTER {
        selectInstruction("ld", $5, 0, $3, "", 0);
    }
|   LD DOLLAR IDENTIFIER COMMA REGISTER {
        selectInstruction("ld", $5, 0, 0, $3, 1);
    }
|   LD imm COMMA REGISTER {
        selectInstruction("ld", $4, 0, $2, "", 2);
    }
|   LD IDENTIFIER COMMA REGISTER {
        selectInstruction("ld", $4, 0, 0, $2, 3);
    }
|   LD REGISTER COMMA REGISTER {
        selectInstruction("ld", $4, $2, 0, "", 4);
    }
|   LD OBRACKET REGISTER CBRACKET COMMA REGISTER {
        selectInstruction("ld", $6, $3, 0, "", 5);
    }
|   LD OBRACKET REGISTER PLUS imm CBRACKET COMMA REGISTER {
        selectInstruction("ld", $8, $3, $5, "", 6);
    }
|   LD OBRACKET REGISTER PLUS IDENTIFIER CBRACKET COMMA REGISTER {
        selectInstruction("ld", $8, $3, 0, $5, 7);
    }
|   ST REGISTER COMMA imm {
        selectInstruction("st", $2, 0, $4, "", 0);
    }
|   ST REGISTER COMMA IDENTIFIER {
        selectInstruction("st", $2, 0, 0, $4, 1);
    }
|   ST REGISTER COMMA OBRACKET REGISTER CBRACKET {
        selectInstruction("st", $2, $5, 0, "", 2);
    }
|   ST REGISTER COMMA OBRACKET REGISTER PLUS imm CBRACKET {
        selectInstruction("st", $2, $5, $7, "", 3);
    }
|   CSRRD CSR COMMA REGISTER {
        selectInstruction("csrrd", $4, 0, 0, $2, 0);
    }
|   CSRWR REGISTER COMMA CSR {
        selectInstruction("csrwr", $2, 0, 0, $4, 0);
    };

%%

int yyerror(const char* s) {
    printf(s);
    return 0;
}
