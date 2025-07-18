#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "minics50.h"

#define BUFFER_SIZE 64

// Turn on for debug allocation prints, do not turn otherwise
// #define DEBUG_PRINTS

static uint16_t __ALLOCATIONS__ = 0;
static string *__str_allocations = NULL;

// Prototipos de funciones
static string _get_string_intern_(const char *prompt);
static bool _SAFE_STRING_ALLOCATION_(char *ptr);
__attribute__((destructor)) static void _FREE_STRING_ALLOCATIONS_();

string get_string(const char *prompt)
{
  string output = NULL;
  do
  {
    output = _get_string_intern_(prompt);
  } while (output == NULL);
  return output;
}

// Obtener input del usuario en forma de string
static string _get_string_intern_(const char *prompt)
{
  printf("%s", prompt);

  // Memoria temporal del stack, se autocolecta
  char temp_memory[BUFFER_SIZE];

  if (fgets(temp_memory, BUFFER_SIZE, stdin))
  {
    // Strip newline
    int length = strlen(temp_memory);
    if (length > 0 && temp_memory[length - 1] == '\n')
      temp_memory[length - 1] = '\0';
  }

  // Retornamos null si el string tiene un tamaño menor o igual a 0
  if (strlen(temp_memory) <= 0)
    return NULL;

  // Ahora crearé memoria dinámica dependiendo del tamaño del input
  // Usamos + 1 porque quiero el null terminator
  char *input = calloc(strlen(temp_memory) + 1, sizeof(char));
#ifdef DEBUG_PRINTS
  printf("Allocated new string space at %p with size %lu\n", input, strlen(temp_memory) + 1);
#endif
  // Copiamos la memoria al nuevo spot
  memcpy(input, temp_memory, strlen(temp_memory) + 1);

  if (_SAFE_STRING_ALLOCATION_(input))
    return input;
  else
  {
    free(input);
    return NULL;
  }
}

string sconcat(const string a, const string b)
{
  uint16_t combined_length = strlen(a) + strlen(b);
  // Sumar 1 para contar el null terminator '\0'
  string new_string = calloc(combined_length + 1, sizeof(char));
#ifdef DEBUG_PRINTS
  printf("Allocated new string space at %p with size %d\n", new_string, combined_length + 1);
#endif
  // Si no se pudo alocar, devolvamos nulo
  if (new_string == NULL)
    return NULL;
  // Ahora vamos a agregar a nuestro nuevo string el string a
  memcpy(new_string, a, strlen(a));
  // Ahora vamos a agregar a nuestro nuevo string el string b
  memcpy(new_string + strlen(a), b, strlen(b));
  // Ponemos el null terminator
  *(new_string + combined_length) = '\0';

  // Finalmente, lo añadimos al alocador, si no se pudo, lo liberamos para evitar leaks
  if (_SAFE_STRING_ALLOCATION_(new_string))
    return new_string;
  else
  {
    free(new_string);
    return NULL;
  }
}

// Esta función guardará un string creado en las alocaciones para su elinicación automática,
// en el caso de la función retornar falso, el string no pudo ser alocado y debe ser eliminado manualmente
static bool _SAFE_STRING_ALLOCATION_(char *ptr)
{
  // Que no nos la intenten colar pasando un nulo
  if (ptr == NULL)
    return false;

  if (__str_allocations != NULL)
  {
    // Tratamos de realocar para una nueva alocación de memoria
    string *new_allocation = realloc(__str_allocations, (__ALLOCATIONS__ + 1) * sizeof(string *));
    // Si la realocación no se pudo conseguir, retornamos -1
    if (!new_allocation)
      return false;
    else
      __str_allocations = new_allocation;
  }
  else
  {
    // Creamos un nuevo arreglo de punteros a string
    __str_allocations = calloc(__ALLOCATIONS__ + 1, sizeof(string *));
    // Si no se pudo crear, retornamos falso
    if (__str_allocations == NULL)
      return false;
  }

  // Si todo funcionó, todo bien, podemos guardar el string y subir las alocaciones
  __str_allocations[__ALLOCATIONS__] = ptr;

#ifdef DEBUG_PRINTS
  printf("string saved to safe allocator index %d (string at %p) '%s'\n", __ALLOCATIONS__, ptr, ptr);
#endif

  __ALLOCATIONS__++;
  return true;
}

// Función automática para la liberación de strings
__attribute__((destructor)) static void _FREE_STRING_ALLOCATIONS_()
{
  for (uint16_t i = 0; i < __ALLOCATIONS__; i++)
  {
#ifdef DEBUG_PRINTS
    printf("string deallocation %d at %p '%s'\n", i, __str_allocations[i], __str_allocations[i]);
#endif
    free(__str_allocations[i]);
  }
  free(__str_allocations);
  __ALLOCATIONS__ = 0;
}