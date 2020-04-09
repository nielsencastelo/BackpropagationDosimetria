#ifndef INSTANCIAS
#define INSTANCIAS
#include <vector>
using namespace std;
class Instancias
{
    public:
        vector <double> exemplos;
        vector<double> prof;
        int lin_exemplos;
        int col_exemplo;
        int lin_prof;
        int col_prof;
    public:
        void lerArquivo();
        void ImprimeExemplos();
        void ImprimeDesejado();
};
#endif // INSTANCIAS_H
