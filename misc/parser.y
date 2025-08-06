%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "../inc/instruction.hpp"

    int yylex(void);
    int yyerror(const char* s);

    void selectInstruction(char*, short, short, uint32, char*);
%}

%union {
    int num;
    short regn;
    char *str;
}

%token <num> NUMBER
%token <regn> REGISTER
%token <str> IDENTIFIER CSR SYMBOL_REF STRING_LITERAL

%token EXTERN GLOBAL SECTION WORD END ASCII SKIP
%token LD ST PUSH POP XCHG
%token ADD SUB MUL DIV NOT AND OR XOR
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
|   SECTION IDENTIFIER { printf("section .%s\n", $2); }
|   WORD word_identifier_list
|   SKIP NUMBER { printf(".skip %d\n", $2); }
|   ASCII string { printf(".ascii %s\n", $2); }
|   END { return 0; };


extern_identifier_list:
    IDENTIFIER { printf(".extern %s\n", $1);}
|   extern_identifier_list COMMA IDENTIFIER { printf(".extern %s\n", $3);}

global_identifier_list:
    IDENTIFIER { printf(".global %s\n", $1);}
|   global_identifier_list COMMA IDENTIFIER { printf(".global %s\n", $3);}

word_identifier_list:
    IDENTIFIER { printf(".word %s\n", $1);}
|   NUMBER { printf(".word %d\n", $1); }
|   word_identifier_list COMMA IDENTIFIER { printf(".word %s\n", $3);}
|   word_identifier_list COMMA NUMBER { printf(".word %d\n", $3);}


label_definition:
    label COLON;


label:
    IDENTIFIER { $$ = $1; printf("%s:\n", $$); };

string:
    QUOTE IDENTIFIER QUOTE { $$ = $2; }
|   STRING_LITERAL { $$ = $1; };

imm:
    NUMBER { $$ = $1; };


instruction:
    HALT {
        selectInstruction("halt", -1, -1, -1, "");
    }
|   INT {
        selectInstruction("int", -1, -1, -1, "");
    }
|   IRET {
        selectInstruction("iret", -1, -1, -1, "");
    }
|   CALL IDENTIFIER {
        selectInstruction("instr", -1, -1, -1, $2);
    }
|   CALL imm {
        selectInstruction("instr", -1, -1, $2, "");
    }
|   RET {
        selectInstruction("ret", -1, -1, -1, "");
    }
|   JMP IDENTIFIER {
        selectInstruction("jmp", -1, -1, -1, $2);
    }
|   BEQ REGISTER COMMA REGISTER COMMA IDENTIFIER {
        selectInstruction("beq", $2, $4, -1, $6);
    }   
|   BNE REGISTER COMMA REGISTER COMMA IDENTIFIER {
        selectInstruction("bne", $2, $4, -1, $6);
    }   
|   BGT REGISTER COMMA REGISTER COMMA IDENTIFIER {
        selectInstruction("bgt", $2, $4, -1, $6);
    }
|   PUSH REGISTER {
        selectInstruction("push", $2, -1, -1, "");
    }
|   POP REGISTER {
        selectInstruction("pop", $2, -1, -1, "");
    }
|   XCHG REGISTER COMMA REGISTER {
        selectInstruction("xchg", $2, $4, -1, "");
    }
|   ADD REGISTER COMMA REGISTER {
        selectInstruction("add", $2, $4, -1, "");
    }
|   SUB REGISTER COMMA REGISTER {
        selectInstruction("sub", $2, $4, -1, "");
    }
|   MUL REGISTER COMMA REGISTER {
        selectInstruction("mul", $2, $4, -1, "");
    }
|   DIV REGISTER COMMA REGISTER {
        selectInstruction("div", $2, $4, -1, "");
    }
|   NOT REGISTER {
        selectInstruction("not", $2, -1, -1, "");
    }
|   AND REGISTER COMMA REGISTER {
        selectInstruction("and", $2, $4, -1, "");
    }
|   OR REGISTER COMMA REGISTER {
        selectInstruction("or", $2, $4, -1, "");
    }
|   XOR REGISTER COMMA REGISTER {
        selectInstruction("xor", $2, $4, -1, "");
    }
|   SHL REGISTER COMMA REGISTER {
        selectInstruction("shl", $2, $4, -1, "");
    }
|   SHR REGISTER COMMA REGISTER {
        selectInstruction("shr", $2, $4, -1, "");
    }
|   LD DOLLAR imm COMMA REGISTER {
        selectInstruction("ld", $5, -1, $3, "");
    }
|   LD DOLLAR IDENTIFIER COMMA REGISTER {
        selectInstruction("ld", $5, -1, -1, $3);
    }
|   LD imm COMMA REGISTER {
        selectInstruction("ld", $4, -1, $2, "");
    }
|   LD IDENTIFIER COMMA REGISTER {
        selectInstruction("ld", $4, -1, -1, $2);
    }
|   LD OBRACKET REGISTER CBRACKET COMMA REGISTER {
        selectInstruction("ld", $6, $3, -1, "");
    }
|   LD OBRACKET REGISTER PLUS imm CBRACKET COMMA REGISTER {
        selectInstruction("ld", $8, $3, $5, "");
    }
|   ST REGISTER COMMA OBRACKET REGISTER CBRACKET {
        selectInstruction("st", $2, $5, -1, "");
    }
|   ST REGISTER COMMA OBRACKET REGISTER PLUS imm CBRACKET {
        selectInstruction("st", $2, $5, $7, "");
    }
|   ST REGISTER COMMA IDENTIFIER {
        selectInstruction("st", $2, -1, -1, $4);
    }
|   CSRRD CSR COMMA REGISTER {
        selectInstruction("csrrd", $4, -1, -1, $2);
    }   
|   CSRWR CSR COMMA REGISTER {
        selectInstruction("csrwr", $4, -1, -1, $2);
    };

%%

int main() {
    printf("Assembly parser started\n");
    yyparse();


    printf("Assembly finished\n");
    return 0;
}

int yyerror(const char* s) {
    printf(s);
}