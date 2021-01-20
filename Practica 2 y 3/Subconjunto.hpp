#ifndef _SUBCONJUNTO_HPP_
#define _SUBCONJUNTO_HPP_

#include <algorithm>
#include "AFN.hpp"
#include "AFD.hpp"
using namespace std;
class Subconjunto{
public:
    Subconjunto(){};
    int subconjuntoActual;
    int kernelActual;
    int estadoInicial;  
    int estadoFinal;
    void iniciarAlgoritmo();
    vector<int> subconjunto;
    vector<int> kernel;
    vector<vector<int> > subconjuntos;
    vector<vector<int> > kernels;
    vector<Transicion*> tablaDePreprocesamiento;
    vector<Transicion*> transicionesAFD;
    vector<Estado*> estadosAFD;
    void generarTablaDePreprocesamiento(AFN afn);
    void procesarEstadoInicial(AFN afn);
    void aplicarCerraduraEpsilon(int numeroDeEstado, vector<Transicion*> tablaDeTransiciones);
    void aplicarCerraduraEpsilon(vector<int> miSubconjunto, vector<Transicion*> tablaDeTransiciones);
    void moverConSimbolo(vector<int> misubconjunto, char simbolo);
    vector<int> obtenerDiferencia(vector<int> subconjunto1, vector<int> subconjunto2);
    bool estaEnElSubconjunto(int estado, vector<int> misubconjunto);
    void agregarSubconjunto();
    void agregarKernel();
    bool existeElSubconjunto();
    bool existeElKernel();
    void imprimirMoverConSimbolo();
    void imprimirCerraduraEpsilon();
    void imprimirTransicionesAFD();
    void agregarTransicionAFD(int estado1, int estado2, char simbolo);
    void generarEstados();
    void generarArchivo(AFN *afn);
    void limpiarVectores();
};

#endif
