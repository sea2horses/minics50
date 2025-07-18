#ifndef MINICS50_H
#define MINICS50_H

typedef char *string;

string get_string(const char *prompt);

/*
sconcat

Esta función crea un nuevo string automáticamente manejado a base de dos strings originales

a -> string
b -> string

Los strings pueden ser c strings (char*) o strings normales, el null terminator se añade de manera automática y se añade al alocador automático de memoria,
por lo que el string resultante no debe ser liberado con free()
*/
string sconcat(const string a, const string b);

#endif