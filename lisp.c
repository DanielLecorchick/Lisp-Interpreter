//Daniel Lecorchick
//CWID: 12295846
//CS503
//Project 1: Lisp Interpreter

//To Compile: gcc lisp.c -lm

//To Run: 
//Regular Interpreter: ./a.out
//Testing Suite: ./a.out --test

//To the grader:
//the code is broken into sections with the /********/ makes it a little easier to read
//i was too lazy to split it into multiple files sorry

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

/*********************************/
/*Begin the basic data structures*/
/*********************************/

struct sExpr;
static struct sExpr* eval(struct sExpr *sexp);

typedef enum {
    // atom types: int, float, symbol, string
    intNum,
    floatNum,
    symbol,
    string,
} AtomType;

typedef struct {
    //number symbol or string
    AtomType type;
    union {
        int intAtom;
        double floatAtom;
        char *strAtom;
    };
} Atom;

typedef enum {
    //types of sExprs are atoms, concells and nil
    atom,
    consCell,
    nil,
} sExprType;

typedef struct sExpr sExpr;

typedef struct {
    //con cells are made of a car(first) and cdr(rest)
    sExpr *car;
    sExpr *cdr;
} ConsCell;

struct sExpr {
    //sExprs are made of atoms and con cells
    sExprType type;
    union {
        Atom atom;
        ConsCell cell; 
    };
};

sExpr *cons(sExpr *car, sExpr *cdr) {
    //utilizes two pointers a car and cdr
    sExpr *s = malloc(sizeof(sExpr));
    s->type = consCell;
    s->cell.car = car;
    s->cell.cdr = cdr;
    return s;
}

sExpr *car(sExpr *s){
    //first item in a list is car
    if (s->type == consCell) return s->cell.car;
    return NULL; 
}

sExpr *cdr(sExpr *s){
    //the rest in a list is cdr
    if (s->type == consCell) return s->cell.cdr;
    return NULL;
}

sExpr *cadr(sExpr *s) {
    //cadr is the cdr of the rest
    return car(cdr(s));
}

/*********************************/
/**End the basic data structures**/
/*********************************/


/*********************************/
/*******Begin the Predicates******/
/*********************************/

bool isNil(sExpr *s){
    //check if sexpr is a nil constant (empty list)
    if (s == NULL) return true;
    if (s->type == nil) return true;
    return false;
}

bool isSymbol(sExpr *s){
    //checks if sexpr is a symbol
    if (s == NULL) return false;
    if (s->type == atom && s->atom.type == symbol) return true;
    return false;
}

bool isNumber(sExpr *s){
    //checks if sexpr is a number, either a int or float
    if (!s) return false;
    return s->type == atom && (s->atom.type == intNum || s->atom.type == floatNum);
}

bool isString(sExpr *s){
    //checks if sexpr is a string
    if (s == NULL) return false;
    if (s->type == atom && s->atom.type == string) return true;
    return false;
}

bool isList(sExpr *s){
    //checks if it is either a con cell or nil constant (empty list)
    if (s->type == consCell || isNil(s)) return true;
    return false;
}

bool isTruesExpr(sExpr *s) {
    //maps any s-exprssion to boolean val
    if(isNil(s)) return false;
    return true;
}

bool eq(sExpr *a, sExpr *b) {
    //checks if two things are rqual 
    if (isNil(a) && isNil(b)) return true;
    if (isNil(a) || isNil(b)) return false;
    if (a->type != b->type) return false;
    if (a->type == atom) {
        if (a->atom.type != b->atom.type) return false;
        if (a->atom.type == intNum) return a->atom.intAtom == b->atom.intAtom;
        if (a->atom.type == floatNum) return fabs(a->atom.floatAtom - b->atom.floatAtom) < 1e-9;
        if (a->atom.type == symbol || a->atom.type == string)
            return strcmp(a->atom.strAtom, b->atom.strAtom) == 0;
    }
    return false;
}

bool null(sExpr *s) {
    return isNil(s);
}

/*********************************/
/********End the Predicates*******/
/*********************************/


/*********************************/
/**Begin the Arithmetic and Logic*/
/*********************************/

sExpr *createIntAtom(int val) {
    //creates a int atom
    sExpr *s = malloc(sizeof(sExpr));
    s->type = atom;
    s->atom.type = intNum;
    s->atom.intAtom = val;
    return s;
}

sExpr *createFloatAtom(double val) {
    //creates a float atom
    sExpr *s = malloc(sizeof(sExpr));
    s->type = atom;
    s->atom.type = floatNum;
    s->atom.floatAtom = val;
    return s;
}

sExpr *createSymbolAtom(const char *sym) {
    //creates a symbol atom
    sExpr *s = malloc(sizeof(sExpr));
    s->type = atom;
    s->atom.type = symbol;
    s->atom.strAtom = malloc(strlen(sym) + 1);
    strcpy(s->atom.strAtom, sym);
    return s;
}

sExpr *createStringAtom(const char *str) {
    //creates a string atom
    sExpr *s = malloc(sizeof(sExpr));
    s->type = atom;
    s->atom.type = string;
    s->atom.strAtom = malloc(strlen(str) + 1);
    strcpy(s->atom.strAtom, str);
    return s;
}

sExpr *createNil() {
    //creates a nil atom
    sExpr *s = malloc(sizeof(sExpr));
    s->type = nil;
    return s;
}

sExpr *createBoolAtom(bool val) {
    //creates bool atoms to convert
    if (val) return createSymbolAtom("True");
    else return createNil();
}

sExpr *add(sExpr *a, sExpr *b) {
    //adds
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;

    if (a->atom.type == floatNum || b->atom.type == floatNum) return createFloatAtom(va + vb);
    else return createIntAtom((int)va + vb);
}

sExpr *sub(sExpr *a, sExpr *b) {
    //subtract
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;
    
    if (a->atom.type == floatNum || b->atom.type == floatNum) return createFloatAtom(va - vb);
    else return createIntAtom((int)va - vb);
}

sExpr *mul(sExpr *a, sExpr *b) {
    //multiply
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;
    
    if (a->atom.type == floatNum || b->atom.type == floatNum) return createFloatAtom(va * vb);
    else return createIntAtom((int)va * vb);
}

sExpr *divi(sExpr *a, sExpr *b) {
    //divsion
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;
    
    if (a->atom.type == floatNum || b->atom.type == floatNum) return createFloatAtom(va / vb);
    else return createIntAtom((int)va / vb);
}

sExpr *mod(sExpr *a, sExpr *b) {
    //modulo
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;

    if (a->atom.type == floatNum || b->atom.type == floatNum) return createFloatAtom(fmod(va, vb));
    else return createIntAtom((int)va % (int)vb);
}

sExpr *lt(sExpr *a, sExpr *b) {
    //lt
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;
    return createBoolAtom(va < vb);
}

sExpr *gt(sExpr *a, sExpr *b) {
    //gt
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;
    return createBoolAtom(va > vb);
}

sExpr *lte(sExpr *a, sExpr *b) {
    //lte
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;
    return createBoolAtom(va <= vb);
}

sExpr *gte(sExpr *a, sExpr *b) {
    //gte
    double va, vb;
    if (a->atom.type == intNum) va = a->atom.intAtom;
    else va = a->atom.floatAtom;
    if (b->atom.type == intNum) vb = b->atom.intAtom;
    else vb = b->atom.floatAtom;
    return createBoolAtom(va >= vb);
}

sExpr *and(sExpr *a, sExpr *b) {
    //and
    return createBoolAtom(isTruesExpr(a) && isTruesExpr(b));
}

sExpr *or(sExpr *a, sExpr *b) {
    //or
    return createBoolAtom(isTruesExpr(a) || isTruesExpr(b));
}

sExpr *not(sExpr *a) {
    //not
    return createBoolAtom(!isTruesExpr(a));
}

/*********************************/
/***End the Arithmetic and Logic**/
/*********************************/


/*********************************/
/*********Begin the Parser********/
/*********************************/

typedef struct {
    //struct for the parser including the input string, current position and length
    char *input;
    int pos;
    int length;
} Parser;

void skipWhitespace(Parser *p) {
    //skips any whitespace chars
    while (p->pos < p->length && isspace(p->input[p->pos])) p->pos++;
}

char peekChar(Parser *p) {
    //a peak function to look at the current character but doesnt go forward
    if (p->pos >= p->length) return '\0';
    else return p->input[p->pos];
}

char getChar(Parser *p) {
    //gets the current char and adcances the position
    if (p->pos >= p->length) return '\0';
    else return p->input[p->pos++];
}

bool isSymbolChar(char c) {
    //checks is the character is for symbols
    return !isspace(c) && c != '(' && c != ')' && c != '"' && c != '\0';
}

sExpr *parseNumber(Parser *p) {
    //parses numbers, checks for - and deals if there is a decimal
    char buffer[256];
    int i = 0;
    bool hasDecimal = false;

    if (peekChar(p) == '-') buffer[i++] = getChar(p);

    while (p->pos < p->length && i < 255) {
        char c = peekChar(p);
        if (isdigit(c)) buffer[i++] = getChar(p); 
        else if (c == '.' && !hasDecimal) {
            hasDecimal = true;
            buffer[i++] = getChar(p);
        }
        else break;
        
    }
    buffer[i] = '\0';

    if (hasDecimal) return createFloatAtom(strtod(buffer, NULL));
    else return createIntAtom(atoi(buffer));
}

sExpr *parseSymbol(Parser *p) {
    //parses symbols
    char buffer[256];
    int i = 0;
    while (p->pos < p->length && i < 255 && isSymbolChar(peekChar(p))) buffer[i++] = getChar(p);
    buffer[i] = '\0';
    return createSymbolAtom(buffer);
}

sExpr *parseString(Parser *p) {
    //parses strings
	char buffer[1000];
	int i = 0;
	getChar(p);
	while (p->pos < p->length && i < 999) {
		char c = peekChar(p);
		if (c == '"') {
			getChar(p);
			break;
		}
        else buffer[i++] = getChar(p);
	}
	buffer[i] = '\0';
	return createStringAtom(buffer);
}

sExpr *parseExpression(Parser *p);

sExpr *parseList(Parser *p) {
    //parses lists
    getChar(p);
    skipWhitespace(p);
    
    if (peekChar(p) == ')') {
        getChar(p);
        return createNil();
    }
    
    sExpr *head = parseExpression(p);
    if (!head) return NULL;
    
    sExpr *current = cons(head, createNil());
    sExpr *result = current;
    
    while (p->pos < p->length) {
        skipWhitespace(p);
        if (peekChar(p) == ')') {
            getChar(p);
            break;
        }
        sExpr *next = parseExpression(p);
        if (!next) break;
        sExpr *newCell = cons(next, createNil());
        current->cell.cdr = newCell;
        current = newCell;
    }
    return result;
}

sExpr *parseExpression(Parser *p) {
    skipWhitespace(p);
    if (p->pos >= p->length) return NULL;
    char c = peekChar(p);
    if (c == '(') return parseList(p);
    else if (c == '\"') return parseString(p);
    else if (c == '\'') {
        getChar(p);
        sExpr *quoted = parseExpression(p);
        return cons(createSymbolAtom("quote"), cons(quoted, createNil()));
    }
    else if (isdigit(c) || (c == '-' && p->pos + 1 < p->length && isdigit(p->input[p->pos + 1]))) return parseNumber(p);
    else if (isSymbolChar(c)) return parseSymbol(p);
    return NULL;
}

Parser *createParser(char *input) {
    //initialize parser with input string
    Parser *p = malloc(sizeof(Parser));
    p->input = input;
    p->pos = 0;
    p->length = strlen(input);
    return p;
}

sExpr *parse(char *input) {
    //parse input string into S-expression
    Parser *p = createParser(input);
    sExpr *result = parseExpression(p);
    free(p);
    return result;
}

/*********************************/
/**********End the Parser*********/
/*********************************/

/*********************************/
/*********Begin Printing**********/
/*********************************/

void printSExpr(sExpr *s) {
    //prints sExprs
    if (!s) {
        printf("NULL");
        return;
    }
    
    if (s->type == atom) {
        if (s->atom.type == intNum) printf("%d", s->atom.intAtom);
        else if (s->atom.type == floatNum) printf("%g", s->atom.floatAtom);
        else if (s->atom.type == symbol) printf("%s", s->atom.strAtom);
        else if (s->atom.type == string) printf("\"%s\"", s->atom.strAtom);
    }
    else if (s->type == consCell) {
        printf("(");
        for (sExpr *current = s; current->type == consCell; current = current->cell.cdr) {
            if (current != s) printf(" ");
            printSExpr(current->cell.car);
            if (current->cell.cdr->type == nil) break;
            if (current->cell.cdr->type == atom) {
                printf(" ");
                printSExpr(current->cell.cdr);
                break;
            }
        }
        printf(")");
    }
    else printf("()");
}

/*********************************/
/**********End Printing***********/
/*********************************/

/*********************************/
/*********Begin Memory************/
/*********************************/

void freeSExpr(sExpr *s) {
    //frees all sExprs to prevent memory issues 
    if (!s) return;
    
    if (s->type == atom) {
        if ((s->atom.type == symbol || s->atom.type == string) && s->atom.strAtom) free(s->atom.strAtom);
    }
    else if (s->type == consCell) {
        freeSExpr(s->cell.car);
        freeSExpr(s->cell.cdr);
    }  
    free(s);
}

static sExpr *deepCopy(sExpr *s) {
    if (!s) return NULL;

    if (s->type == nil) {
        return createNil();
    }
    else if (s->type == atom) {
        if (s->atom.type == intNum) {
            return createIntAtom(s->atom.intAtom);
        }
        else if (s->atom.type == floatNum) {
            return createFloatAtom(s->atom.floatAtom);
        }
        else if (s->atom.type == symbol) {
            return createSymbolAtom(s->atom.strAtom ? s->atom.strAtom : "");
        }
        else if (s->atom.type == string) {
            return createStringAtom(s->atom.strAtom ? s->atom.strAtom : "");
        }
        else {
            return NULL;
        }
    }
    else if (s->type == consCell) {
        return cons(deepCopy(s->cell.car), deepCopy(s->cell.cdr));
    }

    return NULL;
}

/*********************************/
/**********End Memory*************/
/*********************************/


/*********************************/
/***********Begin Eval************/
/*********************************/

//a bunch of functions to make life easy/recommended in class

static sExpr *env = NULL;

static sExpr *list2(sExpr *a, sExpr *b) { 
    return cons(a, cons(b, createNil()));
}

static sExpr *list3(sExpr *a, sExpr *b, sExpr *c) {
    return cons(a, cons(b, cons(c, createNil())));
}

static sExpr *makeFrame(sExpr *symbols, sExpr *values) {
    return cons(symbols, values);
}

static sExpr *frameSymbols(sExpr *frame) {
    return car(frame);
}

static sExpr *frameValues(sExpr *frame) {
    return cdr(frame);
}

static sExpr *envTop() {
    return car(env);
}

static void envPush(sExpr *frame) {
    env = cons(frame, env);
}

static void envPop() {
    env = cdr(env);
}

static bool symEqualsCStr(sExpr *sym, const char *name) {
    return sym && sym->type == atom && sym->atom.type == symbol && strcmp(sym->atom.strAtom, name) == 0;
}

static sExpr *lookup(sExpr *sym) {
    //lookup searches frames from top to bottom
    for (sExpr *stack = env; !isNil(stack); stack = cdr(stack)) {
        sExpr *frame = car(stack);
        sExpr *syms = frameSymbols(frame);
        sExpr *vals = frameValues(frame);

        while (!isNil(syms) && !isNil(vals)) {
            sExpr *s = car(syms);
            if (s->type == atom && s->atom.type == symbol) {
                if (strcmp(s->atom.strAtom, sym->atom.strAtom) == 0)
                    return deepCopy(car(vals));
            }
            syms = cdr(syms);
            vals = cdr(vals);
        }
    }
    return sym;
}

static sExpr *setVar(sExpr *sym, sExpr *value) {
    //set makes cons to the head of the current frames symbol and val lists
    sExpr *oldFrame = envTop();
    sExpr *newSyms = cons(sym, frameSymbols(oldFrame));
    sExpr *newVals = cons(deepCopy(value), frameValues(oldFrame));

    env->cell.car = makeFrame(newSyms, newVals);

    return value;
}

static sExpr *eval(sExpr *sexp);

static sExpr *evalList(sExpr *list) {
    //eval each element of a  list, returns new list of results
    if (isNil(list)) return createNil();
    sExpr *head = eval(car(list));
    sExpr *tail = evalList(cdr(list));
    return cons(head, tail);
}

static sExpr *applyLambda(sExpr *lambdaVal, sExpr *actuals) {
    //applies a lambda function so it works
    sExpr *formals = car(cdr(lambdaVal));
    sExpr *bodies  = cdr(cdr(lambdaVal));
    sExpr *values  = evalList(actuals);
    sExpr *newSyms = createNil();
    sExpr *newVals = createNil();
    sExpr *f = formals, *v = values;

    while (!isNil(f) && !isNil(v)) {
        newSyms = cons(car(f), newSyms);
        newVals = cons(deepCopy(car(v)), newVals);
        f = cdr(f);
        v = cdr(v);
    }
    sExpr *frame = makeFrame(newSyms, newVals);
    envPush(frame);
    sExpr *result = createNil();
    for (sExpr *bs = bodies; !isNil(bs); bs = cdr(bs)) {
        result = eval(car(bs));
    }
    envPop();
    return result;
}

static sExpr *eval(sExpr *sexp) {
    //main eval function redirects to the right function a lot of shit in here 
    if (isNil(sexp)) return createNil();

    if (sexp->type == atom) {
        if (sexp->atom.type == symbol) return lookup(sexp);
        return sexp;
    }

    sExpr *op = car(sexp);
    sExpr *args = cdr(sexp);

    if (op->type == atom && op->atom.type == symbol) {
        if (symEqualsCStr(op, "quote") || symEqualsCStr(op, "'")) return deepCopy(car(args));
        if (symEqualsCStr(op, "exit")) exit(0);
        if (symEqualsCStr(op, "set")) {
            sExpr *name = car(args);
            sExpr *val = eval(car(cdr(args)));
            return setVar(name, val);
        }
        if (symEqualsCStr(op, "define")) {
            sExpr *name = car(args);
            sExpr *second = car(cdr(args));
            if (second && second->type == consCell && car(second)->type == atom && car(second)->atom.type == symbol && symEqualsCStr(car(second), "lambda")) {
                return setVar(name, second);
            }
            else {
                sExpr *val = eval(second);
                return setVar(name, val);
            }
        }
        if (symEqualsCStr(op, "lambda")) return sexp;
        if (symEqualsCStr(op, "if")) {
            sExpr *test = eval(car(args));
            if (isTruesExpr(test)) {
                return eval(car(cdr(args)));
            }
            else {
                return eval(car(cdr(cdr(args))));
            }
        }
        if (symEqualsCStr(op, "and")) {
            sExpr *e1 = eval(car(args));
            if (!isTruesExpr(e1)) return e1;
            return eval(car(cdr(args)));
        }
        if (symEqualsCStr(op, "or")) {
            sExpr *e1 = eval(car(args));
            if (isTruesExpr(e1)) return e1;
            return eval(car(cdr(args)));
        }
        if (symEqualsCStr(op, "cond")) {
            for (sExpr *pairs = args; !isNil(pairs); pairs = cdr(pairs)) {
                sExpr *pair = car(pairs);
                sExpr *test = eval(car(pair));
                if (isTruesExpr(test)) {
                    return eval(car(cdr(pair)));
                }
            }
            return createNil();
        }
        if (symEqualsCStr(op, "map")) {
            sExpr *fn = car(args);
            sExpr *lst = eval(car(cdr(args)));
            sExpr *result_rev = createNil();
        
            for (sExpr *it = lst; !isNil(it); it = cdr(it)) {
                sExpr *call = cons(fn, cons(car(it), createNil()));
                sExpr *val = eval(call);
                result_rev = cons(val, result_rev);
            }
            sExpr *res = createNil();
            for (sExpr *it = result_rev; !isNil(it); it = cdr(it)) {
                res = cons(car(it), res);
            }
            return res;
        }
        if (symEqualsCStr(op, "symbol")) {
            sExpr *a = eval(car(args));
            return createBoolAtom(a->type == atom && a->atom.type == symbol);
        }
        if (symEqualsCStr(op, "number")) {
            sExpr *a = eval(car(args));
            return createBoolAtom(a->type == atom &&
                (a->atom.type == intNum || a->atom.type == floatNum));
        }
        if (symEqualsCStr(op, "string")) {
            sExpr *a = eval(car(args));
            return createBoolAtom(a->type == atom && a->atom.type == string);
        }
        if (symEqualsCStr(op, "list")) {
            sExpr *a = eval(car(args));
            return createBoolAtom(a->type == consCell || a->type == nil);
        }
        if (symEqualsCStr(op, "+") || symEqualsCStr(op, "add")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return add(a,b);
        }
        if (symEqualsCStr(op, "-") || symEqualsCStr(op, "sub")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return sub(a,b);
        }
        if (symEqualsCStr(op, "*") || symEqualsCStr(op, "mul")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return mul(a,b);
        }
        if (symEqualsCStr(op, "/") || symEqualsCStr(op, "div")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return divi(a,b);
        }
        if (symEqualsCStr(op, "%") || symEqualsCStr(op, "mod")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return mod(a, b);
        }
        if (symEqualsCStr(op, "lt") || symEqualsCStr(op, "<")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return lt(a,b);
        }
        if (symEqualsCStr(op, "gt") || symEqualsCStr(op, ">")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return gt(a,b);
        }
        if (symEqualsCStr(op, "lte") || symEqualsCStr(op, "<=")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return lte(a,b);
        }
        if (symEqualsCStr(op, "gte") || symEqualsCStr(op, ">=")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return gte(a,b);
        }
        if (symEqualsCStr(op, "eq")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return createBoolAtom(eq(a,b));
        }
        if (symEqualsCStr(op, "not")) {
            sExpr *a = eval(car(args));
            return not(a);
        }
        if (symEqualsCStr(op, "cons")) {
            sExpr *a = eval(car(args));
            sExpr *b = eval(car(cdr(args)));
            return cons(a,b);
        }
        if (symEqualsCStr(op, "car")) {
            sExpr *a = eval(car(args));
            return car(a);
        }
        if (symEqualsCStr(op, "cdr")) {
            sExpr *a = eval(car(args));
            return cdr(a);
        }
        if (symEqualsCStr(op, "null")) {
            sExpr *a = eval(car(args));
            return createBoolAtom(null(a));
        }
    }
    if (op->type == consCell && car(op)->type == atom && car(op)->atom.type == symbol && symEqualsCStr(car(op), "lambda")) return applyLambda(op, args);
    
    sExpr *opVal = eval(op);
    if (opVal->type == consCell && car(opVal)->type == atom && car(opVal)->atom.type == symbol && symEqualsCStr(car(opVal), "lambda")) return applyLambda(opVal, args);
    
    return cons(eval(op), evalList(args));
}

/*********************************/
/*************End Eval************/
/*********************************/


/*********************************/
/***********Begin Test************/
/*********************************/

//
typedef struct {
    char *input;
    char *expected;
} Test;


char *sExprToString(struct sExpr *s) {
    static char buf[1024];
    buf[0] = '\0'; 
    size_t pos = 0;
    void helper(struct sExpr *x) {
        if (!x) {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "NULL");
            return;
        }
        if (x->type == nil) {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "()");
        }
        else if (x->type == atom) {
            if (x->atom.type == intNum) pos += snprintf(buf + pos, sizeof(buf) - pos, "%d", x->atom.intAtom);
            else if (s->atom.type == floatNum) {
                if (fabs(s->atom.floatAtom - (int)s->atom.floatAtom) < 1e-9) pos += snprintf(buf + pos, sizeof(buf) - pos, "%.1f", s->atom.floatAtom);
                else pos += snprintf(buf + pos, sizeof(buf) - pos, "%g", s->atom.floatAtom);
            }
            else if (x->atom.type == symbol) pos += snprintf(buf + pos, sizeof(buf) - pos, "%s", x->atom.strAtom);
            else if (x->atom.type == string) pos += snprintf(buf + pos, sizeof(buf) - pos, "\"%s\"", x->atom.strAtom);
        }
        else if (x->type == consCell) {
            pos += snprintf(buf + pos, sizeof(buf) - pos, "(");
            struct sExpr *cur = x;
            int first = 1;
            while (cur && cur->type == consCell) {
                if (!first) pos += snprintf(buf + pos, sizeof(buf) - pos, " ");
                helper(cur->cell.car);
                cur = cur->cell.cdr;
                first = 0;
            }
            pos += snprintf(buf + pos, sizeof(buf) - pos, ")");
        }
    }

    helper(s);
    return buf;
}

void runTests(){
    //a comprehensive test suite run by doing ./a.out --test
    //goes over all the types of problems in class and in the 
    Test tests[] = {
        //sprint 1
        {"()", "()"},
        {"42", "42"},
        {"-15", "-15"},
        {"0", "0"},
        {"3.14", "3.14"},
        {"-2.5", "-2.5"},
        {"\"hello\"", "\"hello\""},
        {"\"\"", "\"\""},
        {"\"hello world\"", "\"hello world\""},
        {"'x", "x"},
        {"(quote 42)", "42"},
        {"(quote x)", "x"},
        {"(quote \"abc\")", "\"abc\""},
        {"(quote ())", "()"},
        {"(quote (1 2 3))", "(1 2 3)"},
        {"(quote (a (b c) d))", "(a (b c) d)"},
        {"'(1 2 3)", "(1 2 3)"},
        {"'((a b) (c d))", "((a b) (c d))"},
        {"(cons 1 '(2 3))", "(1 2 3)"},
        {"(cons 5 '())", "(5)"},
        {"(car '(a b c))", "a"},
        {"(cdr '(a b c))", "(b c)"},
        {"(cons 'a '())", "(a)"},
        {"'((a (7 8 9) x) (b (4 5 6) y))", "((a (7 8 9) x) (b (4 5 6) y))"},
        {"(cons \"hi\" '(\"there\"))", "(\"hi\" \"there\")"},
        {"(car '(1))", "1"},
        {"(cdr '(1))", "()"},
        {"(car '((1 2) (3 4)))", "(1 2)"},
        {"(cdr '((1 2) (3 4)))", "((3 4))"},
        {"(cons '(1 2) '(3 4))", "((1 2) 3 4)"},
        {"(cons '(1) '((2) (3)))", "((1) (2) (3))"},
        {"(car '((a b) c d))", "(a b)"},
        {"(cdr '((a b) c d))", "(c d)"},
        {"(cons '() '(1 2 3))", "(() 1 2 3)"},
        {"(cons 'a (cons 'b '()))", "(a b)"},
        {"(car (cons 10 '(20 30)))", "10"},
        {"(cdr (cons 10 '(20 30)))", "(20 30)"},
        {"(cons (cons 1 '(2)) '(3 4))", "((1 2) 3 4)"},
        {"(quote (\"hi\" 1 (2 3) ()))", "(\"hi\" 1 (2 3) ())"},
        {"(cons 1 (cons 2 (cons 3 '())))", "(1 2 3)"},
        {"(car '(\"x\"))", "\"x\""},
        {"(cdr '(\"x\"))", "()"},
        {"(car (cdr '(1 2 3)))", "2"},
        {"(cdr (cdr '(1 2 3)))", "(3)"},
        {"(cdr (cdr (cdr '(1 2 3))))", "()"},
        {"(cons 0 (cons 1 (cons 2 '())))", "(0 1 2)"},
        {"(cons 'x (cons 'y (cons 'z '())))", "(x y z)"},

        //Sprint 2
        {"(null '())", "True"},
        {"(null 5)", "()"},
        {"(null \"hi\")", "()"},
        {"(null '(1))", "()"},
        {"(null (cdr '(a)))", "True"},
        {"(symbol 'abc)", "True"},
        {"(symbol \"abc\")", "()"},
        {"(symbol 123)", "()"},
        {"(symbol ())", "()"},
        {"(number 123)", "True"},
        {"(number -5)", "True"},
        {"(number 0)", "True"},
        {"(number 3.14)", "True"},
        {"(number \"hi\")", "()"},
        {"(string \"abc\")", "True"},
        {"(string \"\")", "True"},
        {"(string 123)", "()"},
        {"(string ())", "()"},
        {"(list '(1 2 3))", "True"},
        {"(list ())", "True"},
        {"(list 5)", "()"},
        {"(list \"hi\")", "()"},
        {"(list (cons 1 '(2 3)))", "True"},
        {"(null (car '()))", "True"},
        {"(symbol '())", "()"},
        {"(number '())", "()"},
        {"(string '())", "()"},
        {"(list '())", "True"},
        {"(symbol 'x)", "True"},
        {"(number -2.5)", "True"},
        {"(list (cdr '(a)))", "True"},
        {"(string (quote hi))", "()"},
        {"(number (quote hi))", "()"},
        {"(symbol (quote hi))", "True"},
        {"(null (list '()))", "()"},
        {"(list (list 1 2))", "True"},
        {"(list (cons 'a '()))", "True"},
        {"(list (cons '() '()))", "True"},
        {"(null (cdr (cdr '(a b)))))", "True"},
        {"(symbol (car '(x y)))", "True"},
        {"(string (car '(\"abc\" \"def\")))", "True"},
        {"(number (car '(1 2 3)))", "True"},
        {"(list (cdr '(1 2)))", "True"},
        {"(symbol (cdr '(1)))", "()"},
        {"(string (cdr '(\"a\")))", "()"},
        {"(number (cdr '(5)))", "()"},
        {"(null (car '(1)))", "()"},
        {"(null (cdr '(1 2)))", "()"},
        {"(null (cdr (cdr '(1))))", "True"},

        //Sprint 3
        {"(add 2 3)", "5"},
        {"(add -5 3)", "-2"},
        {"(add 0 5)", "5"},
        {"(add 1.5 2.5)", "4.0"},
        {"(add 3 2.5)", "5.5"},
        {"(sub 10 4)", "6"},
        {"(sub 3 10)", "-7"},
        {"(sub 5 0)", "5"},
        {"(sub 5.5 2.5)", "3.0"},
        {"(sub 10 2.5)", "7.5"},
        {"(mul 4 5)", "20"},
        {"(mul -3 4)", "-12"},
        {"(mul 0 100)", "0"},
        {"(mul 2.5 4.0)", "10.0"},
        {"(mul 3 2.5)", "7.5"},
        {"(div 20 4)", "5"},
        {"(div 10.0 2.0)", "5.0"},
        {"(div 15 2.0)", "7.5"},
        {"(div -12 3)", "-4"},
        {"(mod 10 3)", "1"},
        {"(mod 10 5)", "0"},
        {"(mod 10.5 3.0)", "1.5"},
        {"(mod -13 5)", "-3"},
        {"(lt 3 5)", "True"},
        {"(lt 5 3)", "()"},
        {"(lt 5 5)", "()"},
        {"(gt 5 3)", "True"},
        {"(gt 3 5)", "()"},
        {"(gt 5 5)", "()"},
        {"(lte 3 5)", "True"},
        {"(lte 5 5)", "True"},
        {"(lte 7 5)", "()"},
        {"(gte 7 5)", "True"},
        {"(gte 5 5)", "True"},
        {"(gte 3 5)", "()"},
        {"(eq 5 5)", "True"},
        {"(eq 5 6)", "()"},
        {"(eq 3.14 3.14)", "True"},
        {"(eq 3.14 2.71)", "()"},
        {"(eq \"hi\" \"hi\")", "True"},
        {"(eq \"hi\" \"bye\")", "()"},
        {"(eq () ())", "True"},
        {"(eq '(1 2) '(1 3))", "()"},
        {"(not True)", "()"},
        {"(not ())", "True"},
        {"(not 5)", "()"},
        {"(not \"hi\")", "()"},
        {"(not (quote ()))", "True"},

        //Sprint 5
        {"(set x 10)", "10"},
        {"x", "10"},
        {"(add x 5)", "15"},
        {"(set y (add 5 5))", "10"},
        {"y", "10"},
        {"(set pi 3.14)", "3.14"},
        {"pi", "3.14"},
        {"(set msg \"hello\")", "\"hello\""},
        {"msg", "\"hello\""},
        {"(set lst (quote (1 2 3)))", "(1 2 3)"},
        {"lst", "(1 2 3)"},
        {"(set x 200)", "200"},
        {"x", "200"},
        {"(set z (sub 50 20))", "30"},
        {"z", "30"},
        {"(set a (mul 6 7))", "42"},
        {"a", "42"},
        {"(set b (div 22 2))", "11"},
        {"b", "11"},
        {"(set c (mod 10 3))", "1"},
        {"c", "1"},
        {"(set str (quote \"abc\"))", "\"abc\""},
        {"str", "\"abc\""},
        {"(set sym 'foo)", "foo"},
        {"sym", "foo"},
        {"(eq 3 (add 1 2))", "True"},
        {"(eq 5 (div 11 2))", "True"},
        {"(quote 42)", "42"},
        {"(quote x)", "x"},
        {"(quote \"hi\")", "\"hi\""},
        {"(quote ())", "()"},
        {"(quote (1 2 3))", "(1 2 3)"},
        {"(quote (a (b c) d))", "(a (b c) d)"},
        {"(quote (1 x \"s\" ()))", "(1 x \"s\" ())"},
        {"'(1 2 3)", "(1 2 3)"},
        {"'((a b) (c d))", "((a b) (c d))"},
        {"(set nested '(1 (2 (3 (4)))))", "(1 (2 (3 (4))))"},
        {"nested", "(1 (2 (3 (4))))"},
        {"(set boolTrue (lt 2 3))", "True"},
        {"boolTrue", "True"},
        {"(set boolFalse (gt 5 10))", "()"},
        {"boolFalse", "()"},
        {"(set n1 100)", "100"},
        {"(set n2 200)", "200"},
        {"(add n1 n2)", "300"},
        {"(sub n2 n1)", "100"},
        {"(mul n1 n2)", "20000"},
        {"(div n2 n1)", "2"},
        {"(quote (cons 1 (cons 2 (cons 3 '())))", "(cons 1 (cons 2 (cons 3 (quote ()))))"},
        {"(set quoted '(a b c))", "(a b c)"},
        {"quoted", "(a b c)"},

        //Sprint 6
        {"(and True True)", "True"},
        {"(and True ())", "()"},
        {"(and () (div 1 0))", "()"},
        {"(and True 5)", "5"},
        {"(and 5 True)", "True"},
        {"(and 5 ())", "()"},
        {"(or True 99)", "True"},
        {"(or () 99)", "99"},
        {"(or () ())", "()"},
        {"(or 5 ())", "5"},
        {"(or () 5)", "5"},
        {"(if (lt 2 3) 100 200)", "100"},
        {"(if (gt 2 3) 100 200)", "200"},
        {"(if (lt 1 2) 111)", "111"},
        {"(if (gt 1 2) 111)", "()"},
        {"(if (eq 5 5) 'yes 'no)", "yes"},
        {"(if (eq 5 6) 'yes 'no)", "no"},
        {"(if True 1 2)", "1"},
        {"(if () 1 2)", "2"},
        {"(cond ((lt 2 3) 111) ((gt 2 3) 222))", "111"},
        {"(cond ((eq 1 2) 333) ((eq 2 2) 444))", "444"},
        {"(cond ((eq 1 2) 'a) ((eq 1 1) 'b))", "b"},
        {"(cond ((eq 1 1) 'a) ((eq 1 1) 'b))", "a"},
        {"(cond (() 'x) (True 'y))", "y"},
        {"(cond (() 'x) (() 'y) (True 'z))", "z"},
        {"(and (lt 2 3) (gt 5 2))", "True"},
        {"(and (lt 2 3) (gt 2 5))", "()"},
        {"(or (lt 2 3) (gt 2 5))", "True"},
        {"(or (gt 5 2) (lt 1 0))", "True"},
        {"(if (and (lt 2 3) (gt 5 2)) 1 0)", "1"},
        {"(if (or (lt 2 1) (eq 4 4)) 1 0)", "1"},
        {"(if (not ()) 42 99)", "42"},
        {"(if (not True) 42 99)", "99"},
        {"(cond ((and True True) 1) ((or True ()) 2))", "1"},
        {"(cond ((and True ()) 1) ((or () ()) 2) (True 3))", "3"},
        {"(if (lt (add 2 2) 5) 'ok 'fail)", "ok"},
        {"(if (gt (mul 3 3) 9) 'ok 'fail)", "fail"},
        {"(if (lte 5 5) 'eq 'no)", "eq"},
        {"(if (gte 5 5) 'eq 'no)", "eq"},
        {"(cond ((eq (mod 10 5) 0) 'div) (True 'no))", "div"},
        {"(cond ((lt (div 10 2) 5) 'small) (True 'big))", "big"},
        {"(and (not ()) (eq 3 3))", "True"},
        {"(or (not ()) (eq 3 4))", "True"},
        {"(if (and True (not ())) 'yes 'no)", "yes"},
        {"(if (or () (and (eq 1 2) True)) 'bad 'ok)", "ok"},
        {"(cond (() 'fail) ((eq 1 2) 'fail) ((eq 2 2) 'pass))", "pass"},
        {"(cond (() 0) (() 1) (() 2) (True 3))", "3"},
        {"(if (and (lt 1 2) (lt 2 3)) 'chain 'no)", "chain"},

        //Sprint 7
        {"(define square (lambda (n) (* n n)))", "(lambda (n) (* n n))"},
        {"(square 9)", "81"},
        {"(define add3 (lambda (x y z) (+ (+ x y) z)))", "(lambda (x y z) (+ (+ x y) z))"},
        {"(add3 1 2 3)", "6"},
        {"(define double (lambda (n) (* 2 n)))", "(lambda (n) (* 2 n))"},
        {"(double 10)", "20"},
        {"(define inc (lambda (n) (+ n 1)))", "(lambda (n) (+ n 1))"},
        {"(inc 7)", "8"},
        {"(define dec (lambda (n) (- n 1)))", "(lambda (n) (- n 1))"},
        {"(dec 7)", "6"},
        {"((lambda (x) (+ x 1)) 5)", "6"},
        {"((lambda (x y) (* x y)) 3 4)", "12"},
        {"((lambda () 42))", "42"},
        {"((lambda (x) (set t (* x 2)) (+ t 5)) 10)", "25"},
        {"(define fact (lambda (n) (if (lte n 1) 1 (* n (fact (- n 1))))))", "(lambda (n) (if (lte n 1) 1 (* n (fact (- n 1)))))"},
        {"(fact 0)", "1"},
        {"(fact 1)", "1"},
        {"(fact 5)", "120"},
        {"(fact 7)", "5040"},
        {"(define fib (lambda (n) (if (lte n 1) n (+ (fib (- n 1)) (fib (- n 2))))))", "(lambda (n) (if (lte n 1) n (+ (fib (- n 1)) (fib (- n 2)))))"},
        {"(fib 0)", "0"},
        {"(fib 1)", "1"},
        {"(fib 6)", "8"},
        {"(fib 8)", "21"},
        {"(define max2 (lambda (a b) (if (gt a b) a b)))", "(lambda (a b) (if (gt a b) a b))"},
        {"(max2 3 5)", "5"},
        {"(max2 9 2)", "9"},
        {"(define min2 (lambda (a b) (if (lt a b) a b)))", "(lambda (a b) (if (lt a b) a b))"},
        {"(min2 3 5)", "3"},
        {"(min2 9 2)", "2"},
        {"(define abs (lambda (n) (if (lt n 0) (- 0 n) n)))", "(lambda (n) (if (lt n 0) (- 0 n) n))"},
        {"(abs -5)", "5"},
        {"(abs 7)", "7"},
        {"(define evenp (lambda (n) (eq (mod n 2) 0)))", "(lambda (n) (eq (mod n 2) 0))"},
        {"(evenp 4)", "True"},
        {"(evenp 7)", "()"},
        {"(define oddp (lambda (n) (not (evenp n))))", "(lambda (n) (not (evenp n)))"},
        {"(oddp 4)", "()"},
        {"(oddp 7)", "True"},
        {"(define id (lambda (x) x))", "(lambda (x) x)"},
        {"(id 42)", "42"},
        {"(id \"hello\")", "\"hello\""},
        {"(id (quote (1 2)))", "(1 2)"},
        {"((lambda (f x) (f x)) inc 10)", "11"},
        {"((lambda (f x) (f x)) square 5)", "25"},
        {"((lambda (f) (f 3)) (lambda (y) (* y y)))", "9"},
        {"((lambda (a b) (add a b)) 100 200)", "300"},


        //Sprint 8
        {"(map (lambda (x) (* x x)) '(1 2 3))", "(1 4 9)"},
        {"(map (lambda (n) (+ n 1)) '(1 2 3 4))", "(2 3 4 5)"},
        {"(map (lambda (x) (if (lt x 0) (- 0 x) x)) '(-3 -2 -1 0 1 2 3))", "(3 2 1 0 1 2 3)"},
        {"(map (lambda (x) (if (eq (mod x 2) 0) 'even 'odd)) '(1 2 3 4 5))", "(odd even odd even odd)"},
        {"(map (lambda (x) (add x 10)) '(5 10 15))", "(15 20 25)"},
        {"(map inc '(10 20 30))", "(11 21 31)"},
        {"(map square '(2 4 6))", "(4 16 36)"},
        {"(map fact '(0 1 2 3 4))", "(1 1 2 6 24)"},
        {"(map fib '(0 1 2 3 4 5 6))", "(0 1 1 2 3 5 8)"},
        {"(map (lambda (xs) (map square xs)) '((1 2) (3 4) (5)))", "((1 4) (9 16) (25))"},
        {"(map (lambda (n) (if (gt n 5) 'big 'small)) '(3 5 7 9))", "(small small big big)"},
        {"(map (lambda (x) (eq x 0)) '(-1 0 1))", "(() True ())"},
        {"(map (lambda (s) (string s)) '(\"a\" \"b\"))", "(True True)"},
        {"(map (lambda (n) (number n)) '(1 2 3 \"x\"))", "(True True True ())"},
        {"(map (lambda (lst) (list lst)) '(() (1) (1 2)))", "(True True True)"},
        {"(map (lambda (x) (add (mul x x) 1)) '(1 2 3))", "(2 5 10)"},
        {"(map (lambda (x) (sub x 1)) '(5 6 7))", "(4 5 6)"},
        {"(map (lambda (x) (div x 2)) '(4 6 8))", "(2 3 4)"},
        {"(map (lambda (x) (mod x 3)) '(3 4 5 6 7))", "(0 1 2 0 1)"},
        {"(map (lambda (x) (lte x 5)) '(4 5 6))", "(True True ())"},
        {"(map (lambda (x) (gte x 5)) '(4 5 6))", "(() True True)"},
        {"(map (lambda (x) (gt x 5)) '(4 5 6))", "(() () True)"},
        {"(map (lambda (x) (lt x 5)) '(4 5 6))", "(True () ())"},
        {"(map (lambda (x) (if (eq x 2) 'hit 'miss)) '(1 2 3))", "(miss hit miss)"},
        {"(map (lambda (x) (if (null x) 'empty 'nonempty)) '(() (1) (2 3)))", "(empty nonempty nonempty)"},
        {"(map (lambda (x) (not x)) '(True () True ()))", "(() True () True)"},
        {"(map (lambda (x) (and x True)) '(True () True ()))", "(True () True ())"},
        {"(map (lambda (x) (or x True)) '(True () True ()))", "(True True True True)"},
        {"(map (lambda (x) (cons x '(z))) '(a b c))", "((42 z) (11 z) (1 z))"},
        {"(map (lambda (x) (car (quote (x y z)))) '(1 2 3))", "(x x x)"},
        {"(map (lambda (x) (cdr (quote (a b)))) '(1 2 3))", "((b) (b) (b))"},
        {"(map (lambda (x) (quote (1 2))) '(a b))", "((1 2) (1 2))"},
        {"(map (lambda (x) x) '(a b c))", "(42 11 1)"},
        {"(map (lambda (x) x) '())", "()"},
        {"(map (lambda (x) (map (lambda (y) (* x y)) '(1 2 3))) '(1 2))", "((1 2 3) (2 4 6))"},
        {"(map (lambda (n) (if (evenp n) 'E 'O)) '(1 2 3 4))", "(O E O E)"},
        {"(map (lambda (n) (if (oddp n) 'O 'E)) '(1 2 3 4))", "(O E O E)"},
        {"(map (lambda (n) (abs n)) '(-3 -2 -1 0 1))", "(3 2 1 0 1)"},
        {"(map (lambda (n) (max2 n 5)) '(3 5 7))", "(5 5 7)"},
        {"(map (lambda (n) (min2 n 5)) '(3 5 7))", "(3 5 5)"},
        {"(map (lambda (x) (gte (add x 1) 3)) '(1 2 3))", "(() True True)"},


        //complex and nested stuff
        {"(add (mul 2 3) 4)", "10"},
        {"(mul (add 2 3) 4)", "20"},
        {"(sub 20 (div 10 2))", "15"},
        {"(add (mul 2 (sub 10 5)) 3)", "13"},
        {"(div (add (mul 3 4) 8) 2)", "10"},
        {"(car (quote (1 2 3)))", "1"},
        {"(cdr (quote (1 2 3)))", "(2 3)"},
        {"(cons 0 (quote (1 2)))", "(0 1 2)"},
        {"(set a 5)", "5"},
        {"(set b 3)", "3"},
        {"(add (mul a b) 2)", "17"},
        {"(set num 10)", "10"},
        {"(add num 5)", "15"},
        {"(set x1 7)", "7"},
        {"(set x2 3)", "3"},
        {"(mul x1 x2)", "21"},
        {"(add (sub (mul 4 5) 8) (div 12 3))", "16"},
        {"(set base 10)", "10"},
        {"(set radius 5)", "5"},

        {NULL, NULL}
    };

    int passed = 0;
    int failed = 0;
    for(int i = 0; tests[i].input; i++){
        sExpr *testCase = parse(tests[i].input);
        sExpr *correctAnswer = eval(testCase);
        char *output = sExprToString(correctAnswer);

        if(strcmp(output, tests[i].expected)==0){
            printf("PASSED! Input: %s Output: %s\n",tests[i].input, output); 
            passed++;
        }
        else {
            printf("\nFAILED! Input: %s Output: %s Expected: %s\n\n", tests[i].input, output, tests[i].expected); 
            failed++;
        }
    }
    printf("\nSummary:\n Passed: %d \n Failed: %d \n", passed, failed);
}

/*********************************/
/*************End Test************/
/*********************************/


int main(int argc, char **argv) {
    // global env with one empty rho
    env = cons(makeFrame(createNil(), createNil()), createNil());
    char input[1024];

    //i know this ascii art mildly looks ai generated but come on all good code had ascii art, dont judge
    printf("     __    _               ____      __                            __           \n");
    printf("    / /   (_)________     /  _/___  / /____  _________  ________  / /____  _____\n");
    printf("   / /   / / ___/ __ \\    / // __ \\/ __/ _ \\/ ___/ __ \\/ ___/ _ \\/ __/ _ \\/ ___/\n");
    printf("  / /___/ (__  ) /_/ /  _/ // / / / /_/  __/ /  / /_/ / /  /  __/ /_/  __/ /    \n");
    printf(" /_____/_/____/ .___/  /___/_/ /_/\\__/\\___/_/  / .___/_/   \\___/\\__/\\___/_/     \n");
    printf("             /_/                              /_/                               \n");
    printf("\n\nUse (exit) or control + C to end the Interpreter\n\n");

    if(argc>1 && strcmp(argv[1],"--test")==0){
        runTests();
    }
    else {
        while(printf("Lisp$ ") && fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = 0;
            if (!strlen(input)) continue;
            
            sExpr *expr = parse(input);
            sExpr *result = eval(expr);
            if (result) {
                printf("> ");
                printSExpr(result);
                printf("\n");
                freeSExpr(result);
            }
            else {
                printf("Try again for the interpreter ./a.out or ./a.out --test for testing \n");
            }
        }
    }
    return 0;
}