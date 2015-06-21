#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <limits>
#include <math.h>
#include <float.h>
#define NUM_SYM 2
#define BASE_SYM 97
#define TOT 1000000
#define EOS vect[NUM_SYM-1]
#define HIGH 1
#define LOW 0

using namespace std;

typedef struct sym
{
    char val; // simbolo numero
    double p; // probabilità
    int end; // fine range valori estrazione
    double end_p; // end range probabilità
    sym(char val , double p, int e , double ep);
    ~sym();
};

typedef struct sym *  psym;

psym * make_source_model(); // genera il modello per la sorgente
int select_sym(psym * vect, int rand); // seleziona il simbolo rappresentato da rand
void encode(int n, psym * vect);
void normalized_encode(int n, psym * vect);
void encode_sym(psym * vect,int curr,double * limits);
char decode_sym(double p, psym *vect, double * limits);
void adaptative_encode(int n, psym * vect);
