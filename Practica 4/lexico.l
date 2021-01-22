%{
	/*
		Un analizador lexico se define usando expresiones regulares.
		Una expresion regular es una notacion simplificada para lenguajes regulares.
		Los lenguajes regulares: conjuntos finitos mas operaciones
			Operaciones: unio, concatenacion y cerradura
	*/
	#include<stdio.h>
	#include<string.h>
	FILE *yyin,*yyout;
	void imprime(char*);
%}

digito [0-9]
Tipos "int"|"float"|"double"|"bool"|"string"|"date"
Database "db_"
Tabla "tabla_"
letras [a-zA-Z]
simbolos [{}() \t;]

fin "Fin\n"|"fin\n"|"FIN\n"

%%

{Database}({letras}|"_")({letras}|{digito}|"_")*							    {printf("-DB-\n");} //Lectura de nombre de base de datos
{Tabla}({letras}|"_")({letras}|{digito}|"_")*								    {printf("-Tabla-\n");} //Lectura de nombre de tabla
({letras}|"_")({letras}|{digito}|"_")*{simbolos}*{Tipos}{simbolos}*{digito}*	{printf("-Campo-\n");} //Lectura de nombre de campos, tipos y tamaños
{fin}																			{return 0;}
%%

int main(int argc, char *argv[])
{
	//yyin = fopen("prueba.txt","r");
	//yyout = fopen("salida.txt","w+");
	yylex(); //Inicia el automata del analizar lexico
	//fclose(yyout);
	//fclose(yyin);
	return 0;
}

void imprime(char* cadena)
{
	char* id;
	char* tipo;
	char* tamano;
	int contador_espacios = 0;
	printf("-Campo-\n");
	
}
