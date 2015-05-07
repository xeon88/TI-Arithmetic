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
    int * ranges= new int [NUM_SYM];


    for(int i=0; i<NUM_SYM;i++)
    {

        rd =(rand() % (tot_prob/NUM_SYM));
        ranges[i]=rd;
        tot_prob -=rd;
    }

    if(tot_prob<TOT) // la somma della cumulata non è uguale alla totalità
    {
        int j=0;
        while(tot_prob>0)
        {
            ranges[j%NUM_SYM]++;
            j++;
            tot_prob--;
        }

    }

    for (int k=0;k<NUM_SYM;k++)
    {
        e=s+ranges[k]-1;
        p=(double)ranges[k]/(double)TOT;
        vect[k]=new sym(BASE_SYM+k,p,s,e);
        s=e+1;
    }



    return vect;
}

psym select_sym(psym * vect)
{
    int rd= rand() % TOT;
    int i=0;

    while(rd<vect[i]->start || rd>vect[i]->end) // cerca in quale simbolo ricade rd
    {
        i++;
    }

    return vect[i];
}


int main(int argc,char** argv)
{

    psym * vect = make_source();
    double totp=0;

    for(int i=0 ; i<NUM_SYM;i++)
    {
        totp +=vect[i]->p;
        cout << "=================== \n";
        cout << "simbolo : " << vect[i]->val << " \n";
        cout << "probabilità : " << vect[i]->p << " \n";
        cout << "start :" << vect[i]->start << "\n";
        cout << "end :" << vect[i]->end << "\n";
        cout << "=================== \n";

    }

    cout << "Probabilità cumulativa :" << totp << "\n";

    int * counters = new int[NUM_SYM];
    psym extract;

    for(int i=0;i<1000;i++)
    {

        extract=select_sym(vect);
        counters[extract->val]++;

    }

    for(int i=0;i<NUM_SYM;i++)
    {
        cout << "simbolo : " << i << "Num estrazioni : " << counters[i] << "\n";
    }

    return 0;
}



