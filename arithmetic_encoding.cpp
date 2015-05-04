#include "arithmetic_encoding.h"


sym::sym(int in, double pin, int s , int e) : val(in) , p(pin) , start(s) , end(e)  {}

psym * make_source()
{

    psym * vect= new psym[NUM_SYM];
    int tot_prob=TOT; // probabilità ancora da assegnare
    double p;  // probabilità del simbolo
    int symb;   // valore simbolo numerico
    int rd; // temporanea per assegnazione probabilità
    int s=0; // inizio range valori estrazione
    int e=0; // fine range valori estrazione
    int cumulative=0 ; // verifica somma probabilità

    for(int i=0; i<NUM_SYM;i++)
    {
        rd = rand() % tot_prob;
        p=(double)rd/TOT;
        symb=BASE_SYM+i;
        tot_prob -=rd;
        e=s+rd;
        s=e+1;
        vect[i]= new sym(symb,p,s,e);
        cumulative +=rd;
    }

    if(cumulative<TOT) // la somma della cumulata non è uguale alla totalità
    {
        int rest=TOT-cumulative;
        double inc=(double)(rest/(TOT*NUM_SYM));
        for (int i=0;i<NUM_SYM;i++)
        {
            vect[i]->p += inc;
        }

    }

    return vect;
}

psym select_sym(psym * vect)
{
    psym selection;
    int rd= rand() % TOT;

    return selection;
}


int main(int argc,char** argv)
{


    return 0;
}
