#include "arithmetic_encoding.h"


sym::sym(char in, double pin, int e, double ep) :
    val(in) , p(pin) , end(e), end_p(ep) {}

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
    int e=0;                                // fine range valori estrazione
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

    double prev=0;

    for (int k=0;k<NUM_SYM;k++)
    {
        e=e+ranges[k];
        ep=(double)e/(double)TOT;
        p=(ep-prev)/double(TOT);
        vect[k]=new sym(BASE_SYM+k,p,e,ep);
        prev=ep;
    }

    return vect;
}


int select_sym(psym * vect, int rand)
{
    int i=0;

    while(rand>vect[i]->end)     // cerca in quale simbolo ricade rd
    {
        i++;
    }

    return i;
}

/*
calcola un valore double il quale esprime la codifica della stringa

*/

void encode_sym(psym * vect , int curr, double * limits)
{

    double range=limits[HIGH]-limits[LOW];
    limits[HIGH]=limits[LOW]+(range)*(vect[curr]->end_p);    // aggiorno estremo sup range prob
    if (curr>0)
    {
        limits[LOW]=limits[LOW]+(range)*(vect[curr-1]->end_p);
    }
         // aggiorno estremo inf prob

}

/*
    Decodifica il messaggio originale avendo in input il double calcolato dall'encoder

*/
char decode_sym(double p,psym * vect,double * limits)
{

    double range;           // dimensione range su cui ricercare p
    int j;
    char sym;
    double low_j;
    double high_j;

    range=limits[HIGH]-limits[LOW];
    j=0;
                                // aggiorno la dimensione del range
    while(p>(high_j=(limits[LOW]+(range)*vect[j]->end_p)))       //  cerco j t.c low(j)< p < high(j)
    {
        j++;
    }

    sym=vect[j]->val;
    limits[HIGH]=limits[LOW]+(range)*(vect[j]->end_p);
    if(j>0)
    {
        limits[LOW]=limits[LOW]+(range)*(vect[j-1]->end_p);
    }

    return sym;
}

/*
    Compone una stringa la quale termina sempre con il simbolo di fine stringa
    definito come ultimo elemento del vettore vect EOS = vect[NUM_SYM-1]
*/

void encode_decode(int n, psym * vect)
{

    double * limit_encoder;
    double * limit_decoder;
    int i;
    int k;
    int j;
    int r;
    int range;
    char * buffer_encoder;
    double * buffer_decoder;
    char * output_decoder;
    int * break_point;
    int curr;
    double encode;
    char decoded;

    limit_encoder=new double[2];
    limit_decoder=new double[2];
    buffer_decoder=new double[n];
    buffer_encoder=new char[n];
    output_decoder=new char[n];
    break_point=new int[n];
    limit_decoder[LOW]=0;
    limit_decoder[HIGH]=1;
    limit_encoder[LOW]=0;
    limit_encoder[HIGH]=1;
    i=0;
    k=0;
    j=0;

    range=TOT-(EOS->end - vect[NUM_SYM-2]->end)+1;


    while(i<n)
    {
        r=rand()% range;
        curr=select_sym(vect,r);
        buffer_encoder[i]=vect[curr]->val;
        if(limit_encoder[HIGH]-limit_encoder[LOW]>1e-10)
        {
            encode_sym(vect,curr,limit_encoder);
        }
        else
        {
            buffer_decoder[j]=(limit_encoder[HIGH]+limit_encoder[LOW])/2;
            limit_encoder[LOW]=0;
            limit_encoder[HIGH]=1;
            j++;
        }

        i++;
    }

    buffer_decoder[j]=(limit_encoder[HIGH]+limit_encoder[LOW])/2;

    j=0;
    double p_range;


    while(buffer_decoder[k]>0)
    {
        p_range=limit_decoder[HIGH]-limit_decoder[LOW];

        if(p_range>1e-10)
        {
            decoded=decode_sym(buffer_decoder[k],vect,limit_decoder);
            output_decoder[j]=decoded;
            j++;
        }
        else
        {
            limit_decoder[LOW]=0;
            limit_decoder[HIGH]=1;
            k++;
        }

    }


    int before_encoding=8*n;
    int after_encoding=k*64;
    double compression=(double)after_encoding/(double)before_encoding;

    cout << "======================================= \n" ;
    cout << "Numero simboli : " << n << " \n";
    cout << "Input in bit : " << before_encoding << "\n";
    cout << "Output in bit : " << after_encoding << "\n\n";
    cout << "Split : " << k << "\n";
    cout << "Compressione " << compression << "\n";
    cout << "======================================= \n" ;
    //cout << "Messaggio inviato  :" << buffer_encoder << "\n";
    //cout << "Messaggio ricevuto :" << output_decoder << "\n";

    free(buffer_decoder);
    free(buffer_encoder);
    free(output_decoder);
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
        cout << "end :" << vect[i]->end_p << "\n";
        cout << "=================== \n";

    }

    int n=10;

    for(int k=1;k<7;k++)
    {
        encode_decode(pow(10,k),vect);
    }

    return 0;
}



