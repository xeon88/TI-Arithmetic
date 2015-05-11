#include "arithmetic_encoding.h"


sym::sym(char in, double pin, int s , int e, double sp, double ep) :
    val(in) , p(pin) , start(s) , end(e) , start_p(sp), end_p(ep) {}

/*
    Funzione per la generazione del modello della sorgente , ovvero i simboli
    e le loro probabilità di emissione
*/


psym * make_source_model()
{

    psym * vect= new psym[NUM_SYM];
    int tot_prob=TOT;                       // probabilità ancora da assegnare
    double p;                               // probabilità del simbolo
    int rd;                                 // temporanea per assegnazione probabilità
    int s=0;                                // inizio range valori estrazione
    int e=0;                                // fine range valori estrazione
    double sp;                              // probabilità iniziale
    double ep;                              // probabilità finale
    int * ranges= new int [NUM_SYM];        // vettore con le dimensione dei range di estrazione


    for(int i=0; i<NUM_SYM;i++)
    {

        rd =(rand() % (tot_prob/NUM_SYM));
        ranges[i]=rd;
        tot_prob -=rd;
    }

    if(tot_prob<TOT)                        // distribuzione dei range non completa
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
        p=(double)ranges[k]/(double)TOT;                // probabilità del simbolo
        ep=(double)e/(double)TOT;
        sp=(double)s/(double)TOT;
        vect[k]=new sym(BASE_SYM+k,p,s,e,sp,ep);
        s=e+1;                                          // aggiorno il valore per lo start
    }



    return vect;
}

psym select_sym(psym * vect, int rand)
{
    int i=0;

    while(rand<vect[i]->start || rand>vect[i]->end)     // cerca in quale simbolo ricade rd
    {
        i++;
    }

    return vect[i];
}

/*
    Compone una stringa la quale termina sempre con il simbolo di fine stringa
    definito come ultimo elemento del vettore vect EOS = vect[NUM_SYM-1]
*/

char* make_message(int n, psym * vect)
{

    int k;                                          // variabile per estrazione int;
    char * message = new char[n];                   // messaggio da produrre
    psym extract;
    int range_sym= TOT-(EOS->end - EOS->start +1);   // escludo il fine stringa
    for(int i=0;i<n-1;i++)                          // compongo casualmente il messaggio
    {
        k=rand()%range_sym;
        extract=select_sym(vect,k);
        message[i]=extract->val;
    }

    message[n-1]=EOS->val;                          // concludo con il fine stringa

    return message;

}

/*
calcola un valore double il quale esprime la codifica della stringa

*/

double encode_message(int n , char * message , psym * vect)
{

    double encode;
    double high_p=1;                            // inizializzo estremo superiore
    double low_p=0;                             // inizializzo estremo inferiore
    double range=0;

    for(int i=0;i<n;i++)
    {
        char curr_char=message[i];
        psym curr_sym=vect[(int)curr_char-BASE_SYM];
        range=high_p-low_p;
        high_p=low_p+(range)*(curr_sym->end_p);            // aggiorno estremo sup range prob
        low_p=low_p+(range)*(curr_sym->start_p);           // aggiorno estremo inf prob
    }

    encode=(high_p+low_p)/2;            // scelgo per semplificità il punto medio dell'intervallo
    return encode;

}

/*
    Decodifica il messaggio originale avendo in input il double calcolato dall'encoder

*/

char * decode_message(double p, int n, psym * vect)
{

    char * message=new char[n];
    double low=0;           // lower bound
    double high=1;          // upper bound
    double range;           // dimensione range su cui ricercare p
    int j;
    double old_low;         // tmp per aggiornamento del lower bound con cui poi calcolo upper
    for(int i=0;i<n;i++)
    {
        j=0;
        range=high-low;                                 // aggiorno la dimensione del range
        while(!((p>low+(range)*vect[j]->start_p) &&
                (p<low+(range)*vect[j]->end_p)) )       //  cerco j t.c low(j)< p < high(j)
        {
            j++;
        }

        message[i]=vect[j]->val;
        old_low=low;
        low=low+(range)*(vect[j]->start_p);
        high=old_low+(range)*(vect[j]->end_p);
    }

    return message;
}

int main(int argc,char** argv)
{

    psym * vect = make_source_model();
    double totp=0;

    for(int i=0 ; i<NUM_SYM;i++)
    {
        totp +=vect[i]->p;
        cout << "=================== \n";
        cout << "simbolo : " << vect[i]->val << " \n";
        cout << "probabilità : " << vect[i]->p << " \n";
        cout << "start :" << vect[i]->start_p << "\n";
        cout << "end :" << vect[i]->end_p << "\n";
        cout << "=================== \n";

    }


    int n=10;
    double encode;
    char * send = make_message(n,vect);
    char * received;

    cout << "Message to encode : " << send << "\n";

    encode=encode_message(n,send,vect);

    cout << "Result encode : " << encode << "\n";

    received=decode_message(encode,n,vect);

    cout << "Message decoded : " << received << "\n";

    return 0;
}



