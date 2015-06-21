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
    double ep=0;                            // probabilità finale
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
        ep=(double)e/TOT;
        p=(ep-prev);
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
calcola i nuovi limiti dell'intervallo normalizzato per la codifica

*/

void encode_sym(psym * vect , int curr, double * limits)
{

    double range=limits[HIGH]-limits[LOW];
    limits[HIGH]=limits[LOW]+(range)*(vect[curr]->end_p);    // aggiorno estremo sup range prob
    if (curr>0)
    {
        limits[LOW]=limits[LOW]+(range)*(vect[curr-1]->end_p); // aggiorno estremo inf prob solo se
    }                                                          // non è il primo carattere


}

/*
    Decodifica un simbolo avendo in input il double calcolato dall'encoder

*/
char decode_sym(double p,psym * vect,double * limits)
{

    double range;           // dimensione range su cui ricercare p
    int j;                  // iteratore
    char sym;               // simbolo decodificato
    double high_j;          // limite superiore del simbolo j

    range=limits[HIGH]-limits[LOW];                             // aggiorno la dimensione del range
    j=0;

    while(p>(high_j=(limits[LOW]+(range)*vect[j]->end_p)))       //  cerco j t.c p < high(j)
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
    simula il processo di codifica e di decodifica senza normalizzazione
    Prima compone il messaggio, successivamente lo codifica seconda
    la procedura della riduzione degli intervalli
    I simboli vengono codificati fino a che non si raggiunge l'underflow
    In tal caso i messaggi vengono codificati a blocchi


*/

void encode(int n, psym * vect)
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
    limit_decoder[LOW]=0;
    limit_decoder[HIGH]=1;
    limit_encoder[LOW]=0;
    limit_encoder[HIGH]=1;
    i=0;
    k=0;
    j=0;

    if(NUM_SYM==2)
    {
        range=TOT;
    }
    if(NUM_SYM>2)
    {
        range=TOT-(EOS->end - vect[NUM_SYM-2]->end)+1;
    }

    while(i<n)
    {
        r=rand()% range;
        curr=select_sym(vect,r);

        buffer_encoder[i]=vect[curr]->val;
        if(i<n-1)
        {
            if(limit_encoder[HIGH]-limit_encoder[LOW]>1e-10)
            {
                encode_sym(vect,curr,limit_encoder);
            }
            else // raggiungimento di underflow
            {
                buffer_decoder[j]=1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
                limit_encoder[LOW]=0;
                limit_encoder[HIGH]=1;
                j++;
            }
        }
        else    // codifica dell'ultimo simbolo
        {
            if(limit_encoder[HIGH]-limit_encoder[LOW]>1e-10)
            {
                encode_sym(vect,NUM_SYM-1,limit_encoder);
                buffer_encoder[i]=EOS->val;
                buffer_decoder[j]=1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
            }
            else
            {
                limit_encoder[HIGH]=1;
                limit_encoder[LOW]=vect[NUM_SYM-2]->end_p;
                buffer_decoder[j]= 1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
            }

        }
        i++;
    }

    buffer_decoder[++j]=0;  // per pulire il buffer della prima posizione non usata

    j=0;
    int before_encoding=log2(NUM_SYM)*n;
    int after_encoding=0;

    while(buffer_decoder[j]>0)
    {
        after_encoding +=(int)buffer_decoder[j]+1;
        j++;
    }

    double compression=(double)after_encoding/(double)before_encoding;
    double rate = (double)after_encoding/(double)n;
    double underflow_sym=(double)n/(double)(k+1);


    cout << "======================================= \n" ;
    cout << "Numero simboli : " << n << " \n";
    cout << "Input in bit : " << before_encoding << "\n";
    cout << "Output in bit : " << after_encoding << "\n";
    cout << "Rate : " << rate << "\n";
    cout << "Compressione : " << compression << "\n";
    cout << "Simboli per underflow : " << underflow_sym << "\n";
    cout << "======================================= \n\n\n" ;

    free(buffer_decoder);
    free(buffer_encoder);
    free(output_decoder);

}


void normalized_encode(int n, psym * vect)
{
    double * limit_encoder;
    int i;
    int j;
    int r;
    int range;
    char * buffer_encoder;
    double * buffer_decoder;
    int curr;
    double encode;
    int count_norm;
    double pr_dimension;

    limit_encoder=new double[2];
    buffer_decoder=new double[n];
    buffer_encoder=new char[n];
    limit_encoder[LOW]=0;
    limit_encoder[HIGH]=1;
    i=0;
    j=0;
    count_norm=0;

    if(NUM_SYM==2)
    {
        range=TOT;
    }
    if(NUM_SYM>2)
    {
        range=TOT-(EOS->end - vect[NUM_SYM-2]->end)+1;
    }

    while(i<n)
    {
        r=rand()% range;
        curr=select_sym(vect,r);
        buffer_encoder[i]=vect[curr]->val;
        pr_dimension=limit_encoder[HIGH]-limit_encoder[LOW];
        if(i<n-1)
        {
            if(pr_dimension>1e-10)
            {
                if(limit_encoder[HIGH]<0.5) // normalizzazione R1 R2
                {
                  limit_encoder[LOW]*=2;
                  limit_encoder[HIGH]*=2;
                  count_norm++;
                }

                if(limit_encoder[LOW]>0.5)
                {
                    limit_encoder[LOW]=2*(limit_encoder[LOW]-0.5);
                    limit_encoder[HIGH]=2*(limit_encoder[HIGH]-0.5);
                    count_norm++;
                }


                encode_sym(vect,curr,limit_encoder);
            }
            else
            {
                buffer_decoder[j]=(double)count_norm + 1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
                limit_encoder[LOW]=0;
                limit_encoder[HIGH]=1;
                j++;
            }
        }
        else
        {
            if(limit_encoder[HIGH]-limit_encoder[LOW]>1e-10)
            {
                encode_sym(vect,NUM_SYM-1,limit_encoder);
                buffer_encoder[i]=EOS->val;
                buffer_decoder[j]=(double)count_norm+1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
            }
            else
            {
                limit_encoder[HIGH]=1;
                limit_encoder[LOW]=vect[NUM_SYM-2]->end_p;
                buffer_decoder[j]=count_norm+1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
            }

        }
        i++;
    }

    buffer_decoder[++j]=0;  // per pulire il buffer della prima posizione non usata

    j=0;

    int before_encoding=log2(NUM_SYM)*n;
    int after_encoding=0;

    while(buffer_decoder[j]>0)
    {
        after_encoding +=(int)buffer_decoder[j]+1;
        j++;
    }

    double compression=(double)after_encoding/(double)before_encoding;
    double rate = (double)after_encoding/(double)n;
    double underflow_sym=(double)n/(double)(j);


    cout << "======================================= \n" ;
    cout << "Numero simboli : " << n << " \n";
    cout << "Input in bit : " << before_encoding << "\n";
    cout << "Output in bit : " << after_encoding << "\n";
    cout << "Rate : " << rate << "\n";
    cout << "Compressione : " << compression << "\n";
    cout << "Simboli per underflow : " << underflow_sym << "\n";
    cout << "======================================= \n\n\n" ;

    free(buffer_decoder);
    free(buffer_encoder);


}


void adaptative_encode(int n, psym * vect)
{
    double * limit_encoder;
    int i;
    int j;
    int r;
    int range;
    char * buffer_encoder;
    double * buffer_decoder;
    int curr;
    double encode;
    double middle;
    double pr_dimension;
    int cnt_0;
    int count_norm;

    limit_encoder=new double[2];
    buffer_decoder=new double[n];
    buffer_encoder=new char[n];
    limit_encoder[LOW]=0;
    limit_encoder[HIGH]=1;
    i=0;
    j=0;
    cnt_0=0;
    count_norm=0;
    double p0;
    double p1;


    if(NUM_SYM==2)
    {
        range=TOT;
    }
    if(NUM_SYM>2)
    {
        range=TOT-(EOS->end - vect[NUM_SYM-2]->end)+1;
    }


    while(i<n)
    {
        r=rand()% range;
        curr=select_sym(vect,r);

        // aggiornamento probabilità;

        if(curr==0)
        {
            cnt_0++;
        }
        p0=((double)(1+ cnt_0))/((double)(2+i+1));
        p1=1-p0;
        vect[0]->p=p0;
        vect[1]->p=p1;

        buffer_encoder[i]=vect[curr]->val;
        pr_dimension=limit_encoder[HIGH]-limit_encoder[LOW];
        if(i<n-1)
        {
            if(pr_dimension>1e-10)
            {
                if(limit_encoder[HIGH]<0.5) // normalizzazione R1 R2
                {
                  limit_encoder[LOW]*=2;
                  limit_encoder[HIGH]*=2;
                  count_norm++;
                }

                if(limit_encoder[LOW]>0.5)
                {
                    limit_encoder[LOW]=2*(limit_encoder[LOW]-0.5);
                    limit_encoder[HIGH]=2*(limit_encoder[HIGH]-0.5);
                    count_norm++;
                }

                encode_sym(vect,curr,limit_encoder);
            }
            else
            {
                buffer_decoder[j]=count_norm+1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
                limit_encoder[LOW]=0;
                limit_encoder[HIGH]=1;
                j++;
            }
        }
        else
        {
            if(limit_encoder[HIGH]-limit_encoder[LOW]>1e-10)
            {
                encode_sym(vect,NUM_SYM-1,limit_encoder);
                buffer_encoder[i]=EOS->val;
            }
            else
            {
                limit_encoder[HIGH]=1;
                limit_encoder[LOW]=vect[NUM_SYM-2]->end_p;
            }
            buffer_decoder[j]=count_norm+1+log2f(1/(limit_encoder[HIGH]-limit_encoder[LOW]));
        }
        i++;
    }

    buffer_decoder[++j]=0;  // per pulire il buffer della prima posizione non usata

    j=0;

    int before_encoding=log2(NUM_SYM)*n;
    int after_encoding=0;

    while(buffer_decoder[j]>0)
    {
        after_encoding +=(int)buffer_decoder[j]+1;
        j++;
    }

    double compression=(double)after_encoding/(double)before_encoding;
    double rate = (double)after_encoding/(double)n;
    double underflow_sym=(double)n/(double)(j);


    cout << "======================================= \n" ;
    cout << "Numero simboli : " << n << " \n";
    cout << "Input in bit : " << before_encoding << "\n";
    cout << "Output in bit : " << after_encoding << "\n";
    cout << "Rate : " << rate << "\n";
    cout << "Compressione : " << compression << "\n";
    cout << "Simboli per underflow : " << underflow_sym << "\n";
    cout << "======================================= \n\n\n" ;

    free(buffer_decoder);
    free(buffer_encoder);


}

int main(int argc,char** argv)
{

    //psym * vect = make_source_model();

    psym * vect = new psym[2];
    vect[0]=new sym(BASE_SYM,0.5,500000,0.5);
    vect[1]=new sym(BASE_SYM+1,0.5,1000000,1);

    double totp=0;
    double source_entropy=0;
    typedef std::numeric_limits< double > dbl;


    for (int i=0;i<NUM_SYM;i++)
    {
        source_entropy +=((vect[i]->p)*log2f(1/vect[i]->p));
    }


    cout.precision(dbl::digits10);
    cout << "Entropia della sorgente : " << source_entropy << "\n\n";


    cout << "==========TEST ENCODE========== \n\n";


    for(int k=1;k<8;k++)
    {
        encode(pow(10,k),vect);
    }



    cout << "==========TEST NORMALIZZAZIONE====== \n\n";


    for(int k=1;k<8;k++)
    {
        normalized_encode(pow(10,k),vect);
    }



    cout << "=========TEST ADAPTATIVE=============  \n\n";

    for(int k=1;k<8;k++)
    {
        adaptative_encode(pow(10,k),vect);
    }

    return 0;
}



