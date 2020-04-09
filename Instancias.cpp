#include "Instancias.hpp"
#include <fstream>
#include <iostream>
void Instancias::lerArquivo() // Cria a instância company do artigo
{
    ifstream txtFile, txtFile2;
    // Abrindo o arquivo
    txtFile.open("treinamento.txt");
    txtFile >> lin_exemplos >> col_exemplo;

    exemplos.resize(lin_exemplos * col_exemplo);

    for(int i = 0; i < lin_exemplos;i++)
    {
        for(int j = 0; j < col_exemplo;j++ )
        {
            txtFile >> exemplos[i * col_exemplo + j];
        }
    }

    txtFile2.open("desejado.txt");
    txtFile2 >> lin_prof >> col_prof;

    prof.resize(lin_prof * col_prof);

    for(int i = 0; i < lin_prof;i++)
    {
        for(int j = 0; j < col_prof;j++ )
        {
            txtFile2 >> prof[i * col_prof + j];
        }
    }
}
void Instancias::ImprimeExemplos()
{
    for(int i = 0; i < lin_exemplos;i++)
    {
        for(int j = 0; j < col_exemplo;j++ )
        {
            cout << exemplos[i * col_exemplo + j] << "  ";
        }
        cout << endl;
    }
}
void Instancias::ImprimeDesejado()
{
    for(int i = 0; i < lin_prof;i++)
    {
        for(int j = 0; j < col_prof;j++ )
        {
            cout << prof[i * col_prof + j] << "  ";
        }
        cout << endl;
    }
}
