#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#define NUM_SYM 20
#define BASE_SYM 0
#define TOT 1000

using namespace std;

typedef struct sym
{
    int val; // simbolo numero
    double p; // probabilità
    int start; // inizio range valori estrazione
    int end; // fine range valori estrazione
    sym(int val , double p, int s , int e);
    ~sym();
};

typedef struct sym *  psym;

psym * make_source();
psym select_sym(psym * vect);
