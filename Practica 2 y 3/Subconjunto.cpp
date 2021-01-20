#include "Subconjunto.hpp"
#include <iostream>
#include <fstream>

void Subconjunto::iniciarAlgoritmo()
{
    int i = 0, k = 0;   // Contadores, i para subconjuntos, k para kernels
    AFN afn("afn.txt","AFN");   // Cargamos un AFN desde archivo
    cout << "Se cargo el siguiente automata AFN:" << endl;
    afn.toString("AFN");
    cout << endl;
    cout << "/*****************************/" << endl << endl;
    cout << "Inicia el algoritmo..." << endl;
    generarTablaDePreprocesamiento(afn);    // Tabla sin transiciones epsilon
    procesarEstadoInicial(afn);    // Se procesa el estado inicial
    while(i < (int)subconjuntos.size())
    {
        for(int j = 0 ; j < (int)afn.automata_alfabeto.size() ; j++)
        {
            moverConSimbolo(subconjuntos[i],afn.automata_alfabeto[j]); // Obtenemos el kernel
            sort(kernel.begin(),kernel.end());  // Ordenamos el kernel
            if(kernel.size() > 0 && !existeElKernel())
            { // Se busca si el kernel ya existe
                cout << "Mover (Q" << i << ", " << afn.automata_alfabeto[j] << ")" << endl;
                agregarKernel(); // Si no existe, se agrega al vector de kernels
                aplicarCerraduraEpsilon(kernels[k],afn.automata_tablaDeTransiciones); // Se aplica la cerradura
                k++; // Incrementamos el contador de kernels
                sort(subconjunto.begin(),subconjunto.end()); // Ordenamos el vector obtenido de la cerradura
                if(!existeElSubconjunto())
                { // Buscamos si existe el subconjunto
                    agregarSubconjunto(); // Si no existe, lo agregamos el vector de subconjuntos
                    agregarTransicionAFD(i,(int)subconjuntos.size()-1,afn.automata_alfabeto[j]); // Agregamos la transicion para el AFD
                }
                else
                { // Si ya existe (a pesar de ser un nuevo kernel)
                    subconjunto.clear();
                    agregarTransicionAFD(i,subconjuntoActual,afn.automata_alfabeto[j]);
                    k--;
                    kernels.erase(kernels.begin()+k); // Eliminamos el ultimo kernel agregado
                }
            }
            else
            { // Si ya existe el kernel
                if(kernel.size() > 0)
                {  // Comprobamos que no sea un kernel sin elementos
                    cout << "Mover (Q" << i << ", " << afn.automata_alfabeto[j] << ")" << endl;
                    imprimirMoverConSimbolo();
                    agregarTransicionAFD(i,kernelActual,afn.automata_alfabeto[j]);
                    kernel.clear();
                }
            }
        }
        i++; // Avanzamos al siguiente estado (subconjunto)
    }
    generarEstados();   // Se llena el vector de estados para el AFD
    generarArchivo(&afn);   // Se crea el archivo para que se pueda cargar el AFD
    limpiarVectores();  // Limpiamos los atributos de la clase
    cout << "Termino el algoritmo." << endl << endl;
    cout << "/*****************************/" << endl << endl;
    cout << "Cargue el AFD desde archivo." << endl;
}

void Subconjunto::procesarEstadoInicial(AFN afn)
{
    estadoInicial = afn.obtenerEstadoInicial()->numeroDeEstado; // Obtenemos el numero del estado inicial
    estadoFinal = afn.obtenerEstadoFinal()->numeroDeEstado;    // Obtenemos el numero del estado final
    aplicarCerraduraEpsilon(estadoInicial,afn.automata_tablaDeTransiciones);    // A partir de aqui inicia el algoritmo
    sort(subconjunto.begin(),subconjunto.end()); // Ordenamos el subconjunto obtenido de la cerradura
    agregarSubconjunto(); // Lo agregamos al vector de subconjuntos
}

void Subconjunto::limpiarVectores()
{
    estadosAFD.clear();
    transicionesAFD.clear();
    tablaDePreprocesamiento.clear();
    subconjunto.clear();
    subconjuntos.clear();
    kernel.clear();
    kernels.clear();
}

void Subconjunto::generarArchivo(AFN *afn)
{
    ofstream archivo("afd.txt");
    if(archivo.is_open()){
        int j;
        j = afn->automata_alfabeto.size();
        archivo << j << endl;
        for(int i = 0 ; i < j ; i++)
        {
            archivo << afn->automata_alfabeto[i];
                if(i != j-1)
                    archivo << ',';
                else
                    archivo << endl;
        }
        j = estadosAFD.size();
        archivo << j-1 << endl;
        for(int i = 0 ; i < j ; i++)
        {
            if(estadosAFD[i]->esFinal)
            {
                archivo << i ;
                if(i != j-1)
                    archivo << ',';
                else
                    archivo << endl;
            }
        }
        j = transicionesAFD.size();
        archivo <<  j << endl;
        int transicionact = 0;
        for(int i = 0 ; i < j ; i++)
        {
            if(transicionact != transicionesAFD[i]->estadoActual || i == 0)
                transicionact = transicionesAFD[i]->estadoActual;
            archivo << transicionact << "->" << transicionesAFD[i]->estadoDeTransicion << ' ';
            archivo << transicionesAFD[i]->simboloDeTransicion << endl;
        }
    }
}

void Subconjunto::generarEstados()
{
    for(int i = 0 ; i < (int)subconjuntos.size() ; i++)
    {
        estadosAFD.reserve(1);
        if(estaEnElSubconjunto(estadoFinal,subconjuntos[i]))
        {
            // El estado sera final si contiene al estado final del AFN
            estadosAFD.push_back(new Estado(i,true));
        }
        else
        {
            // De lo contrario es no final
            estadosAFD.push_back(new Estado(i,false));
        }
    }
}

void Subconjunto::agregarTransicionAFD(int estado1, int estado2, char simbolo)
{
    transicionesAFD.reserve(1);
    transicionesAFD.push_back(new Transicion(estado1,estado2,simbolo));
}

bool Subconjunto::existeElSubconjunto()
{
    int j = subconjuntos.size();
    vector<int> diferencia;
    vector<int>::iterator it;
    for(int i = 0 ; i < j ; i++)
    {
        if(subconjunto.size() == subconjuntos[i].size())
        {
            diferencia.reserve(subconjunto.size()*2);
            it = set_difference(subconjunto.begin(),subconjunto.end(),subconjuntos[i].begin(),subconjuntos[i].end(),diferencia.begin());
            diferencia.resize(it-diferencia.begin());
            if(diferencia.size() == 0)
            {
                subconjuntoActual = i;
                return true;
            }
            else
                diferencia.clear();
        }
    }
    return false;
}

bool Subconjunto::existeElKernel()
{
    int j = kernels.size();
    vector<int> diferencia;
    vector<int>::iterator it;
    for(int i = 0 ; i < j ; i++)
    {
        if(kernel.size() == kernels[i].size())
        {
            diferencia.reserve(kernel.size()*2);
            it = set_difference(kernel.begin(),kernel.end(),kernels[i].begin(),kernels[i].end(),diferencia.begin());
            diferencia.resize(it-diferencia.begin());
            if(diferencia.size() == 0)
            {
                kernelActual = i+1;
                return true;
            }
            else
                diferencia.clear();
        }
    }
    return false;
}

void Subconjunto::agregarSubconjunto()
{
    subconjuntos.reserve(1);
    subconjuntos.push_back(subconjunto);
    imprimirCerraduraEpsilon();
    subconjunto.clear();
}

void Subconjunto::agregarKernel()
{
    kernels.reserve(1);
    kernels.push_back(kernel);
    imprimirMoverConSimbolo();
    kernel.clear();
}

void Subconjunto::imprimirCerraduraEpsilon()
{
    cout << "\tCerradura-E: {";
    for(int i = 0 ; i < (int)subconjunto.size() ; i++){
        if(i != (int)subconjunto.size()-1)
            cout << subconjunto[i] << ",";
        else
            cout << subconjunto[i];
    }
    cout << "}" << endl;
}

void Subconjunto::imprimirMoverConSimbolo()
{
    cout << "\tKernel: {";
    for(int i = 0 ; i < (int)kernel.size() ; i++)
    {
        if(i != (int)kernel.size()-1)
            cout << kernel[i] << ",";
        else
            cout << kernel[i];
    }
    cout << "}" << endl;
}

void Subconjunto::generarTablaDePreprocesamiento(AFN afn)
{
    for(int i = 0 ; i < (int)afn.automata_tablaDeTransiciones.size() ; i++)
    {
        if(!afn.automata_tablaDeTransiciones[i]->esTransicionEpsilon)
        {
            tablaDePreprocesamiento.reserve(1);
            tablaDePreprocesamiento.push_back(afn.automata_tablaDeTransiciones[i]);
        }
    }
}

void Subconjunto::moverConSimbolo(vector<int> misubconjunto, char simbolo)
{
    for(int i = 0 ; i < (int)misubconjunto.size() ; i++)
    {
        for(int j = 0 ; j < (int)tablaDePreprocesamiento.size() ; j++)
        {
            if(tablaDePreprocesamiento[j]->estadoActual == misubconjunto[i])
                if(tablaDePreprocesamiento[j]->simboloDeTransicion == simbolo)
                    if(!estaEnElSubconjunto(tablaDePreprocesamiento[j]->estadoDeTransicion,kernel))
                    {
                        kernel.reserve(1);
                        kernel.push_back(tablaDePreprocesamiento[j]->estadoDeTransicion);
                    }
        }
    }
}

bool Subconjunto::estaEnElSubconjunto(int estado, vector<int> misubconjunto)
{
    int j = misubconjunto.size();
    for(int i = 0 ; i < j ; i++)
        if(misubconjunto[i] == estado)
            return true;
    
    return false;
}

void Subconjunto::aplicarCerraduraEpsilon(int numeroDeEstado, vector<Transicion*> tablaDeTransiciones)
{
    int i;
    int k = tablaDeTransiciones.size();
    if(!estaEnElSubconjunto(numeroDeEstado,subconjunto))
    {
        subconjunto.reserve(1);
        subconjunto.push_back(numeroDeEstado);
    }
    for(i = 0 ; i < k ; i++\)
    {
        if(tablaDeTransiciones[i]->estadoActual == numeroDeEstado)
          if(tablaDeTransiciones[i]->simboloDeTransicion == 'E')
            if(!estaEnElSubconjunto(tablaDeTransiciones[i]->estadoDeTransicion,subconjunto))
                aplicarCerraduraEpsilon(tablaDeTransiciones[i]->estadoDeTransicion,tablaDeTransiciones);
            
    }
}

void Subconjunto::aplicarCerraduraEpsilon(vector<int> miSubconjunto, vector<Transicion*> tablaDeTransiciones)
{
    int i;
    int k = tablaDeTransiciones.size();
    for(int j = 0 ; j < (int)miSubconjunto.size() ; j++)
    {
        if(!estaEnElSubconjunto(miSubconjunto[j],subconjunto))
        {
            subconjunto.reserve(1);
            subconjunto.push_back(miSubconjunto[j]);
        }
    }
    for(int j = 0 ; j < (int)miSubconjunto.size() ; j++)
    {
        for(i = 0 ; i < k ; i++)
        {
            if(tablaDeTransiciones[i]->estadoActual == miSubconjunto[j])
                if(tablaDeTransiciones[i]->simboloDeTransicion == 'E')
                    if(!estaEnElSubconjunto(tablaDeTransiciones[i]->estadoDeTransicion,subconjunto))
                        aplicarCerraduraEpsilon(tablaDeTransiciones[i]->estadoDeTransicion,tablaDeTransiciones);
        }
    }
}
