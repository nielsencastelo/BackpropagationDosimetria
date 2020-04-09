/*
    Algoritmo Backpropagation que utiliza parametros de desemepnho:
    Momento normalizado (alfa) proposto por Pansalkar (1994).
    Taxa de aprendizado baseado (eta adaptativo) em Demuth(1992).
    Autor: Nielsen Castelo Damasceno
    Data: 28/12/2007
*/
#include <iostream>
#include<conio.h>
#include<cstdlib>
#include<cmath>
#include<ctime>
#include "Instancias.hpp"

using namespace std;

// Variáveis globais
const unsigned int nc_max = 6, nn_max = 21, nptr_max = 15;
int threshold = -1;
float er_max = 0.00001;
unsigned int nciclos_max = 50000;
unsigned int nc, nptr;
unsigned int nn[nc_max], fan[nc_max];
float ptre[nn_max+1][nptr_max];
float de[nn_max+1][nptr_max];
float ptr[nn_max+1][nptr_max];
float d[nn_max+1][nptr_max];
float sdw[nn_max+1][nptr_max];
char careta;
float eta, alfa;
float er_total;
unsigned int counter;
float w[nc_max][nn_max+1][nn_max+1];
float no[nc_max][nn_max+1][nptr_max];
float dwanterior[nc_max][nn_max+1][nn_max+1];
Instancias database;

unsigned int nciclos;

// Protótipos de funções
void projeto();
void contexto();
void desempenho();
void modo_inicializacao();
void irparafrente(float no[nc_max][nn_max+1][nptr_max]);
void incpesosran();
float calculaerro();
void retroagir();
void etadaptativo(float x1, float x2);
void teste();

int main()
{
    setlocale(LC_ALL,"Portuguese");

    float x1, x2;
    char testar;

    counter = 0;
    projeto();
    contexto();
    desempenho();
    modo_inicializacao();

    nciclos = 0;
    x1 = 0;
    x2 = 0;

    system("cls");

    cout << "REDES NEURAIS ARTIFICIAS\n";

    do
    {
        irparafrente(no);
        calculaerro();
        x2 = er_total;

        if((careta == 'S')|| (careta == 's'))
        {
            etadaptativo(x1,x2);
        }else if(er_total > er_max)
            retroagir();

        x1 = x2;

        nciclos = nciclos + 1;
        cout << "epoca: " << nciclos << endl;

    }while(er_total > er_max && nciclos < nciclos_max);

    cout << "Erro total: " << er_total << endl;
    cout << "Número de ciclos: " << nciclos << endl;


    cout << endl;
    do
    {
        teste();
        cout << "\n\nQuer testar outra vez <S/N> ?";
        cin >> testar;
    }while((testar == 'S') || (testar == 's'));

    return 0;
}
float calculaerro() // Calcula o erro na saída da RNA multicamadas
{
    unsigned int p, q, s;
    float er_local, er_p[nn_max+1][nptr_max];

    for(q = 1; q <= nptr;q++)
    {
        for(s = 1; s <= nn[nc];s++)
        {
            er_local = d[s][q] - no[nc][s][q];
            er_p[s][q] = 0.5 * pow(er_local,2);
        }
    }
    er_total = 0;
    for(p = 1; p <= nptr;p++)
    {
        for(s = 1; s <= nn[nc];s++)
            er_total = er_total + er_p[s][p];
    }
    return er_total;
}
void desempenho() // Disponibiliza mecanismo para melhoria do desempenho no treinamento da rede neural
{
    char caraux;
    //clrscr();
    cout << "Deseja ativar o método do ETA ADAPATIVO <S/N> ? ";
    cin >> careta;
    if((careta=='s') || (careta=='S'))
    {
        cout << "\n   Escolha o ETA inicial = (Sugestão: 0.01)";
        cin >> eta;
    }
    else
    {
        cout << "\n   Escolha o ETA Constante = ";
        //cin >> eta;
        eta = 0.01;
    }
    cout << endl;

    cout << "Deseja ativar o método do MOMENTO NORMALIZADO <S,N> ? (Sugestão: 0.95)";
    cin >> caraux;
    if((caraux=='S')||(caraux == 's'))
    {
        cout << "\n    Escolha a constante ALFA (0<=ALFA<1) = ";
        cin >> alfa;
    }
    else
        alfa = 0.95;
}
void inicpesosran()// inicializa randomicamente os pesos sinapticos
{
    srand (time(NULL));
    unsigned int r, s, t;
    for(r = 1; r < nc; r++)
    {
        for(s = 1; s <= nn[r+1];s++)
        {
            for(t = 1; t <= (nn[r]+1); t++)
            {
                w[r][s][t] = static_cast <float> (rand()) / static_cast<float> (RAND_MAX);// Pesos entre -0.50 e + 0.50
            }
        }
    }
}
void inicpesosdet()// Inicializa deterministicamente os pesos sinapticos
{
    unsigned  int r, s, t;
    for(r = 1; r < nc; r++)
    {
        for(s = 1; s <= nn[r+1];s++)
        {
            for(t = 1; t <= (nn[r]+1);t++)
            {
                cout << "W[" << r << "] [" << s << "] [" << t << "] = ";
                cin >> w[r][s][t];
            }
        }
    }
}
// escolha do método pra inicialização dos pesos sinapticos
// defalt (Randomico)
void modo_inicializacao()
{
    char caraux;

     system("cls");

    if(counter == 0)
    {
        cout << "Digite <R> para Pesos Randômicos\n";
        cout << "Digite <D> para Pesos Deterministicos\n\n";
        cout << "Qual o modo de Inicializacao? ";
        cin >> caraux;
        if((caraux=='D') || (caraux=='d'))
            inicpesosdet();
        else
            inicpesosran();
    }else
    {
        cout << "Digite <R> para Pesos Randômicos\n";
        cout << "Digite <D> para Pesos Deterministicos\n";
        cout << "Digite <T> para Pesos Treinamento Anterior\n";
        cout << "Qual o modo de Inicializacão? ";
        cin >> caraux;
        if((caraux=='T') || (caraux=='t'))
            cout << "     "; // repete o anterior
        else if ((caraux == 'D') || (caraux == 'd'))
            inicpesosdet();
        else
            inicpesosran();
    }
}

// Rede Evolui da entrada para a saída - Forward
void irparafrente(float no[nc_max][nn_max+1][nptr_max])
{
    unsigned int q,r,s,t;
    float nep, net[nc_max][nn_max+1][nptr_max];

    for( r = 1; r < nc; r++)
    {
        for(s = 1;s <= nn[r+1];s++)
        {
            for(t = 1; t <= nptr;t++)
            {
                net[r][s][t] = 0;
            }
        }
    }

    for( r = 1; r < nc; r++)
    {
        for(q = 1;q <= nptr;q++)
        {
            for(s = 1; s <= nn[r+1];s++)
            {
                for(t = 1; t <= (nn[r]+1); t++)
                {
                    net[r][s][q] = net[r][s][q] + w[r][s][t] * no[r][t][q];
                }
            }
        }


        if(fan[r]==1)
        {
            for(q = 1; q <= nptr; q++)
            {
                for(s = 1; s <= nn[r+1];s++)
                    no[r+1][s][q] = net[r][s][q];
                if(r+1 < nc) no[r+1][s][q] = threshold;
            }
        }

        if(fan[r]==2)
        {
            for(q = 1; q <= nptr;q++)
            {
                for(s = 1; s <= nn[r+1];s++)
                {
                    nep = exp(-net[r][s][q]);
                    no[r+1][s][q] = 1.0 / (1.0 + nep);
                }
                if(r+1 < nc )no[r+1][s][q] = threshold;
            }
        }

        if(fan[r]==3)
        {
            for(q = 1; q <= nptr; q++)
            {
                for(s = 1; s <= nn[r+1]; s++)
                {
                    nep = exp(-net[r][s][q]);
                    no[r+1][s][q] = (1.0 - nep)/ (1.0 + nep);
                }
                if(r+1 < nc) no[r+1][s][q] = threshold;
            }
        }
    }// Fim do for
}

// Calcula a soma dos produtos delta * w
void somadeltaw(unsigned int r, float aux[nn_max+1][nptr_max])
{
    unsigned int q,s,t;

    for(q = 1; q <= nptr;q++)
    {
        for(s = 1; s <= (nn[r]+1); s++)
        {
            sdw[s][q] = 0;
        }
    }
    for(q = 1; q <= nptr; q++)
    {
        for(s = 1; s <= (nn[r]+1); s++)
        {
            for(t = 1; t <= nn[r+1];t++)
            {
                sdw[s][q] = sdw[s][q] + aux[t][q] * w[r][t][s];
            }
        }
    }
}

// Incrementa os pesos sinapticos
void incpeso(unsigned int r, float aux[nn_max+1][nptr_max])
{
    unsigned int q,s,t;
    float dwatual[nc_max][nn_max+1][nn_max+1];

    for( s = 1; s <= nn[r+1];s++)
    {
        for(t = 1; t <= (nn[r]+1); t++)
        {
            for(q = 1; q <= nptr; q++)
            {
                dwatual[r][s][t] = eta * aux[s][q] * no[r][t][q];
                w[r][s][t] = w[r][s][t] + (1.0 - alfa) * dwatual[r][s][t] + (alfa * dwanterior[r][s][t]);
                dwanterior[r][s][t] = dwatual[r][s][t];
            }
        }
    }
}

// Ajusta os pesos através da retropropagação do erro - backward
void retroagir()
{
    unsigned int q,r,s,t;
    float aux[nn_max+1][nptr_max];

    for(r = 1; r < nc; r++)
    {
        for(s = 1; s <= nn[r+1]; s++)
        {
            for(t = 1; t <= (nn[r]+1); t++)
            {
                dwanterior[r][s][t] = 0;
            }
        }
    }
    r = nc - 1;

    if(fan[r] == 1)
    {
        for(q = 1; q <= nptr; q++)
        {
            for(s = 1; s <= nn[nc]; s++)
            {
                aux[s][q] = d[s][q] - no[nc][s][q];
            }
        }
    }

    if(fan[r]==2)
    {
        for(q = 1; q <= nptr; q++)
        {
            for(s = 1; s <= nn[nc]; s++)
            {
                aux[s][q] = ((d[s][q] - no [nc][s][q]) * no[nc][s][q] * (1 - no[nc][s][q]));
            }
        }
    }

    if(fan[r]==3)
    {
        for(q = 1; q <= nptr; q++)
        {
            for(s = 1; s <= nn[nc]; s++)
            {
                aux[s][q] = (0.5 * (d[s][q] - no[nc][s][q]) * (1.0 - pow(no[nc][s][q],2.0)));
            }
        }
    }

    somadeltaw(r,aux);
    incpeso(r,aux);

    for(r = nc - 2; r >= 1; r--)
    {
        if(fan[r]== 1)
        {
            for(q = 1; q <= nptr;q++)
            {
                for(s = 1; s <= nn[r+1];s++)
                {
                    aux[s][q] = sdw[s][q];
                }
            }
        }

        if(fan[r]== 2)
        {
            for(q = 1; q <= nptr;q++)
            {
                for(s = 1; s <= nn[r+1];s++)
                {
                    aux[s][q] = no[r+1][s][q] * (1 - no[r+1][s][q]) * sdw[s][q];
                }
            }
        }

        if(fan[r]== 3)
        {
            for(q = 1; q <= nptr;q++)
            {
                for(s = 1; s <= nn[r+1];s++)
                {
                    aux[s][q] = (0.5 * (1.0 - pow(no[r+1][s][q],2.0))) * sdw[s][q];
                }
            }
        }

        if (r > 1)
            somadeltaw(r,aux);
        incpeso(r,aux);
    }
}

// Implementa o Eta Adaptativo
void etadaptativo(float x1, float x2)
{
    if(x2 <= 1.04 * x1)
    {
        if(x2 >= x1)
            retroagir();
        else
        {
            retroagir();
            eta = 1.05 * eta;
        }
    }else
        eta = 0.7 * eta;
}

// Realiza o teste de treinamento
void teste()
{
    unsigned int s;

    cout << "\nEntre com o exemplo de teste";

    for(s = 1; s <= nn[1];s++)
    {
        cout << "\n    entrada[" << s << "] = ";
        cin >> no[1][s][1];
    }
    no[1][nn[1]+1][1] = threshold;

    irparafrente(no);

    for(s = 1; s <= nn[nc];s++)
    {
        cout << "  saída[" << s << "] = " << no[nc][s][1];
    }
}
// Especificação da Arquitetura da RNA
void projeto()
{
    unsigned int r;

    cout << "Número de camada (Máximo " << nc_max << " camadas)";
    cin >> nc;

    cout << endl;

    for(r = 1; r <= nc; r++)
    {
        cout << "Número de neurônios na camada " << r;
        if(r == 1)  cout << "(Camada de Entrada)";
        if(r == nc) cout << "(Camada de Saída)   ";
        if(r != 1 && r != nc) cout << "              ";
        cout << "-->" << "NN[" << r << "] = ";
        cin >> nn[r];
    }

    system("cls");

    cout << endl;
    cout << "Escolha a Funcão de Ativacao dos Neurônios\n";
    cout << "         " << "Digite 1 para funcao linear\n";
    cout << "         " << "Digite 2 para funcao Sigmóide Unipolar\n";
    cout << "         " << "Digite 3 para funcao Sigmóide Bipolar\n";
    cout << endl;

    for(r = 1; r < nc; r++)
    {
        cout << "Funcão de Ativacao da Camada de Neurônios " << (r + 1) << " = ";
        cin >> fan[r];
    }
}

// Obtenção dos dados para treinamento da RNA
void contexto()
{
    char caraux1, caraux2;
    unsigned int m,p,r,s,t,i,j;
    unsigned int conjunto[nptr_max]; // argumento igual ao nptr máximo

    system("cls");
    cout << "Treinamento via <T>eclado ou <A>rquivo - Disponivel <T> ? ";
    cin >> caraux1;

    if((caraux1 == 'A') || (caraux1 == 'a'))
    {
        database.lerArquivo();

        nptr = database.lin_exemplos;
        for(p = 1, i = 0; p <= nptr; p++,i++)
        {
            for(j = 0, s = 1; s <= nn[1]; s++, j++)
            {
                ptre[s][p] = database.exemplos[i * database.col_exemplo + j];
                //cout << ptre[s][p] << endl;
            }
            ptre[nn[1] + 1][p] = threshold;
        }
        cout << "  Leu exemplos com sucesso ...\n";

        for(i = 0, p = 1; p <= nptr; p++, i++)
        {
            for(j = 0, s = 1; s <= nn[nc]; s++, j++)
            {
                de[s][p] = database.prof[i * database.col_prof + j];
                //cout << de[s][p] << endl;
            }
        }
        cout << "  Leu desejado com sucesso ...\n";

    }else
    {
        cout << "  Número de exemplos de treinamento = ";
        cin >> nptr;


        cout << "\n   Digite os valores iniciais dos exemplos de treinamento\n";
        for(p = 1; p <= nptr; p++)
        {
            cout << endl;
            for(s = 1; s <= nn[1]; s++)
            {
                cout << "    ptre[" << s << ',' << p << "] = ";
                cin >> ptre[s][p];
            }
            ptre[nn[1] + 1][p] = threshold;
            cout << "       ptre[" << (nn[1]+1) << ',' << p << "] = " << threshold;
            cout << endl;
        }

        cout << "\n  Digite os valores das saidas desejadas\n";
        for(p = 1; p <= nptr; p++)
        {
            for(s = 1; s <= nn[nc]; s++)
            {
                cout << "\n      de[" << s << ',' << p << "] = ";
                cin >> de[s][p];
            }
        }
    } // Fechamento do caraux1

    //system("cls");

    cout << "Entrada <R>andômica ou <S>equencial ? ";
    cin >> caraux2;

    if((caraux2 == 'R') || (caraux2=='r'))
    {
        for(m = 1; m <= nptr;m++)
            conjunto[m] = m;

        t = 1;

        while(t <= nptr)
        {
            /* initialize random seed: */
            srand (time(NULL));

            r = rand() % nptr + 1; // range 1 to nptr

            for( m = 1; m <= nptr; m++)
            {
                if(r == conjunto[m])
                {
                    for(s = 1; s <= (nn[1]+1); s++) ptr[s][r] = ptre[s][t];
                    for(s = 1; s <= nn[nc]; s++) d[s][r] = de[s][t];

                    t = t + 1;
                    conjunto[m] = 0;
                }
            }
        }
    }else
    {
        for(s = 1; s <= nptr; s++)
        {
            for(r = 1; r <= (nn[1] + 1); r++) ptr[r][s] = ptre[r][s];
            for(r = 1; r <= nn[nc]; r++) d[r][s] = de[r][s];
        }
    } // Fechamento do caraux2

    for( p = 1; p <= nptr; p++)
    {
        for(s = 1; s <= (nn[1] + 1); s++)
        {
            no[1][s][p] = ptr[s][p];  // Atribuicão da primeira camada
        }
    }
}

