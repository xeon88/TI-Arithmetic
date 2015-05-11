#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#define NUM_SYM 10
#define BASE_SYM 99
#define TOT 1000
#define EOS vect[NUM_SYM-1]

using namespace std;

typedef struct sym
{
    char val; // simbolo numero
    double p; // probabilità
    int start; // inizio range valori estrazione
    int end; // fine range valori estrazione
    double start_p; // start range probabilità
    double end_p; // end range probabilità
    sym(char val , double p, int s , int e , double sp, double ep);
    ~sym();
};

typedef struct sym *  psym;

psym * make_source_model(); // genera il modello per la sorgente
psym select_sym(psym * vect, int rand); // seleziona il simbolo rappresentato da rand
char* make_message(int n, psym * symbols); // genera il messaggio casuale;
