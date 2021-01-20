#include "AFN.hpp"
#include <fstream>

using namespace std;

bool AFN::cambiarDeEstadoAFN(vector<Estado*> estados, vector<Transicion*> tablaDeTransiciones, Estado* estadoActual, string cadena, char simbolo, int indice)
{
    int i;
    Estado* estadoDeTransicion;
    int k = tablaDeTransiciones.size();
    bool cambio, resultado = false, tieneTransicion = false;
    if(indice < (int)cadena.size())
    {
        indice++;
        for(i = 0; i < k; i++)
        {
            if(tablaDeTransiciones[i]->estadoActual == estadoActual->numeroDeEstado)
            {
                if(tablaDeTransiciones[i]->esTransicionEpsilon)
                { // Si hay transicion epsilon
                    tieneTransicion = true;
                    indice--;
                    cout << "\tδ(q" << estadoActual->numeroDeEstado << ", E)";
                    cout << " = q" << tablaDeTransiciones[i]->estadoDeTransicion << endl;
                    estadoDeTransicion = obtenerEstado(tablaDeTransiciones[i]->estadoDeTransicion);
                    cambio = cambiarDeEstadoAFN(estados, tablaDeTransiciones, estadoDeTransicion, cadena, cadena[indice],indice);
                    resultado = resultado || cambio;
                    indice++;
                }
                else if(tablaDeTransiciones[i]->simboloDeTransicion == simbolo)
                { // Si hay trnasicion con el simbolo
                    tieneTransicion = true;
                    cout << "\tδ(q" << estadoActual->numeroDeEstado << "," << simbolo << ")";
                    cout << " = q" << tablaDeTransiciones[i]->estadoDeTransicion << endl;
                    estadoDeTransicion = obtenerEstado(tablaDeTransiciones[i]->estadoDeTransicion);
                    cambio = cambiarDeEstadoAFN(estados, tablaDeTransiciones, estadoDeTransicion, cadena, cadena[indice],indice);
                    //cout << "Termina recursion con epsilon" << endl;
                    resultado = resultado || cambio;
                }
            }
            else if(!tieneTransicion && i == k-1)
            {// Si no tiene transiciones
                cout << "\tδ(q" << estadoActual->numeroDeEstado << "," << simbolo << ")";
                cout << " = M" << endl;
                return false;
            }
        }
        return resultado;
    }
    else
    {
        for(i = 0; i < k; i++)
        { // Se busca si se puede mover con epsilon a partir del estado actual
            if(tablaDeTransiciones[i]->estadoActual == estadoActual->numeroDeEstado)
            {
                if(tablaDeTransiciones[i]->esTransicionEpsilon)
                { // Si tiene transicion epsilon
                    tieneTransicion = true;
                    cout << "\tδ(q" << estadoActual->numeroDeEstado << ", E)";
                    cout << " = q" << tablaDeTransiciones[i]->estadoDeTransicion << endl;
                    estadoDeTransicion = obtenerEstado(tablaDeTransiciones[i]->estadoDeTransicion);
                    cambio = cambiarDeEstadoAFN(estados, tablaDeTransiciones, estadoDeTransicion, cadena, cadena[indice],indice);
                    resultado = resultado || cambio;
                }
            }
        }
        if(tieneTransicion)
            return resultado;
        else
            return estadoActual->esFinal;
    }
}

Estado* AFN::obtenerEstado(int numeroDeEstado)
{
    int k = automata_estados.size();
    for(int i = 0 ; i < k ; i++)
    {
        if(automata_estados[i]->numeroDeEstado == numeroDeEstado)
          return automata_estados[i];
    }
    Estado* muerto = new Estado(-1,false);
    return muerto;
}

void AFN::agregarTransicion(AFN* afn, int estado1, int estado2, char simbolo, bool esEpsilon)
{
  afn->automata_tablaDeTransiciones.reserve(1);
  afn->automata_tablaDeTransiciones.push_back(new Transicion(estado1,estado2,simbolo,esEpsilon));
}

bool AFN::buscarEnAlfabeto(AFN* afn, char c)
{
    int k = afn->automata_alfabeto.size();
    for(int i = 0 ; i < k ; i++)
    {
        if(c == afn->automata_alfabeto[i])
          return true;
    }
    return false;
}

void AFN::agregarSimboloAlAlfabeto(AFN* afn, char c)
{
    if(!buscarEnAlfabeto(afn,c))
    {
        afn->automata_alfabeto.reserve(1);
        afn->automata_alfabeto.push_back(c);
    }
}

void AFN::copiarAlfabeto(AFN* afn, AFN subafn)
{
    int k = subafn.automata_alfabeto.size();
    char c;
    for(int i = 0 ; i < k ; i++)
    {
        c = subafn.automata_alfabeto[i];
        if(!buscarEnAlfabeto(afn,c))
        {
          afn->automata_alfabeto.reserve(1);
          afn->automata_alfabeto.push_back(c);
        }
    }
}

void AFN::eliminarEstadoFinal(AFN *afn)
{
    int j = afn->automata_estados.size();
    for(int i = 0 ; i < j ; i++)
    {
        if(afn->automata_estados[i]->esFinal)
          afn->automata_estados[i]->esFinal = false;
    }
}

void AFN::eliminarEstadoInicial(AFN *afn)
{
    int j = afn->automata_estados.size();
    Estado *estado = afn->obtenerEstadoInicial();
    int numeroDeEstado = estado->numeroDeEstado;
    for(int i = 0 ; i < j ; i++)
    {
        if(afn->automata_estados[i]->numeroDeEstado == numeroDeEstado)
        {
            afn->automata_estados.erase(afn->automata_estados.begin()+i);
            break;
        }
    }
}

void AFN::concatenar(AFN* afn, char simbolo, bool esEpsilon)
{
    Estado* estado1;
    Estado* estado2;
    int estadosDelAutomata = afn->automata_estados.size();
    if(estadosDelAutomata==0)
    {
        estado1 = new Estado(0,false);
        estado2 = new Estado(1,true);
        afn->automata_estados.reserve(2);
        afn->automata_estados.push_back(estado1);
        afn->automata_estados.push_back(estado2);
        afn->automata_estadoInicial = estado1;
        agregarTransicion(afn, estado1->numeroDeEstado, estado2->numeroDeEstado, simbolo, esEpsilon);
    }
    else
    {
        estado2 = new Estado(estadosDelAutomata, true);
        estado1 = afn->obtenerEstadoFinal();
        agregarTransicion(afn, estado1->numeroDeEstado, estado2->numeroDeEstado, simbolo, esEpsilon);
        eliminarEstadoFinal(afn);
        afn->automata_estados.reserve(1);
        afn->automata_estados.push_back(estado2);
    }
    afn->automata_estadoFinal = estado2;
}

void AFN::renumerarEstado(AFN *afn, int antiguo, int nuevo)
{
    int k = afn->automata_estados.size();
    for(int i = 0 ; i < k ; i++)
    {
        if(afn->automata_estados[i]->numeroDeEstado == antiguo)
        {
          afn->automata_estados[i]->numeroDeEstado = nuevo;
          afn->automata_estadoInicial = afn->automata_estados[i];
          break;
        }
    }
}

void AFN::modificarTransiciones(AFN* afn, int antiguo, int nuevo)
{
    int k = afn->automata_tablaDeTransiciones.size();
    for(int i = 0 ; i < k ; i++)
    {
        if(afn->automata_tablaDeTransiciones[i]->estadoActual == antiguo)
          afn->automata_tablaDeTransiciones[i]->estadoActual = nuevo;
        if(afn->automata_tablaDeTransiciones[i]->estadoDeTransicion == antiguo)
          afn->automata_tablaDeTransiciones[i]->estadoDeTransicion = nuevo;
    }
}

void AFN::swapTransiciones(AFN* afn, int estado1, int estado2)
{
    int k = afn->automata_tablaDeTransiciones.size();
    for(int i = 0 ; i < k ; i++)
    {
        if(afn->automata_tablaDeTransiciones[i]->estadoActual == estado1)
          afn->automata_tablaDeTransiciones[i]->estadoActual = estado2;
        else if(afn->automata_tablaDeTransiciones[i]->estadoActual == estado2)
          afn->automata_tablaDeTransiciones[i]->estadoActual = estado1;
        if(afn->automata_tablaDeTransiciones[i]->estadoDeTransicion == estado1)
          afn->automata_tablaDeTransiciones[i]->estadoDeTransicion = estado2;
        else if(afn->automata_tablaDeTransiciones[i]->estadoDeTransicion == estado2)
          afn->automata_tablaDeTransiciones[i]->estadoDeTransicion = estado1;
    }
}

void AFN::agregarTransiciones(AFN* afn, vector<Transicion*> tablaDeTransiciones)
{
    int k = tablaDeTransiciones.size();
    afn->automata_tablaDeTransiciones.reserve(k);
    for(int i = 0 ; i < k ; i++)
        afn->automata_tablaDeTransiciones.push_back(tablaDeTransiciones[i]);
    
}

void AFN::agregarEstados(AFN* afn, vector<Estado*> estados)
{
    int k = estados.size();
    afn->automata_estados.reserve(k);
    for(int i = 0 ; i < k ; i++)
        afn->automata_estados.push_back(estados[i]);
}

void AFN::renumerarEstados(AFN* subafn, int numero)
{
    int k = subafn->automata_tablaDeTransiciones.size();
    for(int i = 0 ; i < k ; i++)
    { // Renumeracion en la tabla de transiciones
        subafn->automata_tablaDeTransiciones[i]->estadoDeTransicion += numero;
        subafn->automata_tablaDeTransiciones[i]->estadoActual += numero;
    }
    k = subafn->automata_estados.size();
    for(int i = 0 ; i < k ; i++) // Renumeracion en el vector de estados
    subafn->automata_estados[i]->numeroDeEstado += numero;
}

void AFN::agregarTransicionEpsilon(AFN* afn, Estado* estado1,Estado* estado2)
{
    Transicion* t = new Transicion(estado1->numeroDeEstado,estado2->numeroDeEstado,'E',true);
    afn->automata_tablaDeTransiciones.reserve(1);
    afn->automata_tablaDeTransiciones.push_back(t);
}

void AFN::unir(AFN* afn, AFN subafn)
{
    Estado* estado1; // Estados adicionales para realizar la union
    Estado* estado2;
    Estado* aux1; // Estados auxiliares
    Estado* aux2;
    int numeroDeEstados = afn->automata_estados.size();
    renumerarEstados(&subafn, numeroDeEstados); // Renumeramos los estados del subafn
    agregarEstados(afn, subafn.automata_estados); // Agregamos los estados del subafn al afn
    agregarTransiciones(afn, subafn.automata_tablaDeTransiciones);  // Agregamos las transiciones del subafn al afn
    numeroDeEstados += (int)subafn.automata_estados.size(); // Se actualiza el valor del numero de estados
    estado1 = new Estado(numeroDeEstados,false);  // Instanciamos los estados necesarios para la union
    estado2 = new Estado(numeroDeEstados+1,true); // Uno de ellos es estado final
    aux1 = afn->obtenerEstadoInicial(); // Obtenemos la referencia del estado inicial del afn
    aux2 = subafn.obtenerEstadoInicial(); // Obtenemos la referencia del estado inicial de afn
    agregarTransicionEpsilon(afn,estado1,aux1); // Se agrega la transicion de estado1 a aux1
    agregarTransicionEpsilon(afn,estado1,aux2); // Se agrega la transicion de estado1 a aux2
    for(int i = 0 ; i < numeroDeEstados ; i++)
    {
        if(afn->automata_estados[i]->esFinal)
            agregarTransicionEpsilon(afn,afn->automata_estados[i],estado2);
    }
    eliminarEstadoFinal(afn); // Los estados finales ahora son no finales
    afn->automata_estados.reserve(2);
    afn->automata_estados.push_back(estado1); // Se agrega el estado inicial
    afn->automata_estados.push_back(estado2); // Se agrega el estado final
    afn->automata_estadoInicial = estado1;
    afn->automata_estadoFinal = estado2;
}

void AFN::aplicarCerraduraKleenAFN(AFN *afn)
{
    Estado* estado1; // Estados adicionales necesarios para la cerradura
    Estado* estado2;
    Estado* aux1; // Estados auxiliares
    Estado* aux2;
    int numeroDeEstados = afn->automata_estados.size();
    aux1 = afn->obtenerEstadoInicial(); // Se obtienen las referencias de loos estados final e inicial
    aux2 = afn->obtenerEstadoFinal();
    estado1 = new Estado(numeroDeEstados,false);  // Se instancian los nuevos estados
    estado2 = new Estado(numeroDeEstados+1,true);
    agregarTransicionEpsilon(afn,aux2,aux1);  // Se agregan las transiciones de la cerradura de Kleen
    agregarTransicionEpsilon(afn,estado1,estado2);
    agregarTransicionEpsilon(afn,estado1,aux1);
    agregarTransicionEpsilon(afn,aux2,estado2);
    eliminarEstadoFinal(afn); // El estado final ahora es no final
    afn->automata_estados.reserve(2); // Se agregan los estados adicionales
    afn->automata_estados.push_back(estado1);
    afn->automata_estados.push_back(estado2);
    afn->automata_estadoInicial = estado1; // Se asignan los nuevos estados final e inicial
    afn->automata_estadoFinal = estado2;
}

void AFN::aplicarCerraduraKleen(AFN* afn, char c, bool esEpsilon){
    Estado* estado1; // Estados adicionales necesarios para la cerradura
    Estado* estado2;
    Estado* estado3;
    Estado* estado4;
    int numeroDeEstados = afn->automata_estados.size();
    if(numeroDeEstados == 0)
    { // Si el AFN no tiene estados
        estado1 = new Estado(0,false); // Instanciamos los estados
        estado2 = new Estado(1,false);
        estado3 = new Estado(2,false);
        estado4 = new Estado(3,true);
        afn->automata_estadoInicial = estado1; // Asignamos el estado inicial
        agregarTransicionEpsilon(afn,estado1,estado2);  // Se agregan las transiciones necesarias
        agregarTransicion(afn, estado2->numeroDeEstado, estado3->numeroDeEstado, c, esEpsilon);
        agregarTransicionEpsilon(afn,estado3,estado4);
        agregarTransicionEpsilon(afn,estado3,estado2);
        agregarTransicionEpsilon(afn,estado1,estado4);
        afn->automata_estadoFinal = estado4; // Asignamos el estado final
        afn->automata_estados.reserve(4); //Agregamos los estados
        afn->automata_estados.push_back(estado1);
        afn->automata_estados.push_back(estado2);
        afn->automata_estados.push_back(estado3);
        afn->automata_estados.push_back(estado4);
    }
    else
    { // Si el automata no esta vacio
        estado1 = new Estado(numeroDeEstados,false);  // Instanciamos los estados
        estado2 = new Estado(numeroDeEstados+1,false);
        estado3 = new Estado(numeroDeEstados+2,true);
        estado4 = afn->obtenerEstadoFinal();  // Obtenemos el estado final del AFN
        agregarTransicionEpsilon(afn,estado4,estado1);  // Agregamos las transiciones necesarias
        agregarTransicion(afn, estado1->numeroDeEstado, estado2->numeroDeEstado, c, esEpsilon);
        agregarTransicionEpsilon(afn,estado2,estado3);
        agregarTransicionEpsilon(afn,estado2,estado1);
        agregarTransicionEpsilon(afn,estado4,estado3);
        eliminarEstadoFinal(afn); // El estado final del AFN ahora es no final
        afn->automata_estadoFinal = estado3;  // Asignamos el nuevo estado final
        afn->automata_estados.reserve(3); // Agregamos los estados nuevos
        afn->automata_estados.push_back(estado1);
        afn->automata_estados.push_back(estado2);
        afn->automata_estados.push_back(estado3);
    }
}

void AFN::aplicarCerraduraPositivaAFN(AFN *afn)
{
    Estado* estado1;  // Estados adicionales necesarios para la cerradura
    Estado* estado2;
    Estado* aux1; // Estados auxiliares
    Estado* aux2;
    int numeroDeEstados = afn->automata_estados.size();
    aux1 = afn->obtenerEstadoInicial(); // Obtenemos las referencias de los estados final e inicial
    aux2 = afn->obtenerEstadoFinal();
    estado1 = new Estado(numeroDeEstados,false); // Instanciamos los nuevos estados
    estado2 = new Estado(numeroDeEstados+1,true);
    agregarTransicionEpsilon(afn,aux2,aux1); // Aregamos las transiciones necesarias
    agregarTransicionEpsilon(afn,estado1,aux1);
    agregarTransicionEpsilon(afn,aux2,estado2);
    eliminarEstadoFinal(afn); // El estado final del AFN ahora es no final
    afn->automata_estados.reserve(2); // Agregamos los nuevos estados
    afn->automata_estados.push_back(estado1);
    afn->automata_estados.push_back(estado2);
    afn->automata_estadoInicial = estado1;  // Se asignan los nuevos estados final e inicial
    afn->automata_estadoFinal = estado2;
}

void AFN::aplicarCerraduraPositiva(AFN* afn, char c, bool esEpsilon)
{
    Estado* estado1; // Estados adicionales necesarios para la cerradura
    Estado* estado2;
    Estado* estado3;
    Estado* estado4;
    int numeroDeEstados = afn->automata_estados.size();
    if(numeroDeEstados == 0)
    { // Si el automata no tiene estados
        estado1 = new Estado(0,false); // Instanciamos los nuevos estados
        estado2 = new Estado(1,false);
        estado3 = new Estado(2,false);
        estado4 = new Estado(3,true);
        agregarTransicionEpsilon(afn,estado1,estado2); // Agregamos las transiciones necesarias
        agregarTransicion(afn, estado2->numeroDeEstado, estado3->numeroDeEstado, c, esEpsilon);
        agregarTransicionEpsilon(afn,estado3,estado4);
        agregarTransicionEpsilon(afn,estado3,estado2);
        afn->automata_estadoInicial = estado1;  // Asignamos los nuevos estados final e inicial
        afn->automata_estadoFinal = estado4;
        afn->automata_estados.reserve(4); // Agregamos los nuevos estados
        afn->automata_estados.push_back(estado1);
        afn->automata_estados.push_back(estado2);
        afn->automata_estados.push_back(estado3);
        afn->automata_estados.push_back(estado4);
    }
    else
    { // Si el automata no es vacio
        estado1 = new Estado(numeroDeEstados,false); // Instanciamos los nuevos estados
        estado2 = new Estado(numeroDeEstados+1,false);
        estado3 = new Estado(numeroDeEstados+2,true);
        estado4 = afn->obtenerEstadoFinal();  // Obtenemos la referencia del estado final del AFN
        agregarTransicionEpsilon(afn,estado4,estado1);  // Agregamos las transiciones necesarias
        agregarTransicion(afn, estado1->numeroDeEstado, estado2->numeroDeEstado, c, esEpsilon);
        agregarTransicionEpsilon(afn,estado2,estado3);
        agregarTransicionEpsilon(afn,estado2,estado1);
        eliminarEstadoFinal(afn); // El estado final del AFN ahora es no final
        afn->automata_estadoFinal = estado3;  // Asignamos el nuevo estado final
        afn->automata_estados.reserve(3); // Agregamos los nuevos estados
        afn->automata_estados.push_back(estado1);
        afn->automata_estados.push_back(estado2);
        afn->automata_estados.push_back(estado3);
    }
}
void AFN::generarArchivoAFN(AFN afn)
{
    int i,j,estadoInicial;
    ofstream archivo("afn.txt");
    if(archivo.is_open())
    {
        j = afn.automata_alfabeto.size();
        archivo << j << endl;
        for(i = 0 ; i < j ; i++)
            archivo << afn.automata_alfabeto[i] << endl;
        j = afn.automata_estados.size();
        archivo << j << endl;
        estadoInicial = afn.obtenerEstadoInicial()->numeroDeEstado;
        archivo << estadoInicial << endl;
        for(i = 0 ; i < j ; i++)
        {
          if(afn.automata_estados[i]->numeroDeEstado != estadoInicial)
          {
            archivo << afn.automata_estados[i]->numeroDeEstado << endl;
            if(afn.automata_estados[i]->esFinal)
              archivo << "s" << endl;
            else
              archivo << "n" << endl;
          }
        }
        j = afn.automata_tablaDeTransiciones.size();
        archivo << j << endl;
        for(i = 0 ; i < j ; i++)
        {
          if(afn.automata_tablaDeTransiciones[i]->esTransicionEpsilon)
          {
            archivo << "s" << endl;
            archivo << automata_tablaDeTransiciones[i]->estadoActual << endl;
            archivo << automata_tablaDeTransiciones[i]->estadoDeTransicion << endl;
          }
          else
          {
            archivo << "n" << endl;
            archivo << automata_tablaDeTransiciones[i]->estadoActual << endl;
            archivo << automata_tablaDeTransiciones[i]->estadoDeTransicion << endl;
            archivo << automata_tablaDeTransiciones[i]->simboloDeTransicion << endl;
          }
        }
    }
}

