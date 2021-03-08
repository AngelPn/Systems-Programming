/* Types used in multiple modules */
#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h> 
#include <stdio.h>

/* enumeration to determine the key type of hash table in mylib */
enum key_type {String, Integer} KeyType;

/*pointer to function that compares 2 values a and b and returns:
  <0 if a<b
  0 if a==b
  >0 if a>b
*/
typedef int (*CompareFunc)(void *a, void *b);

/* pointer to function that destroys an item */
typedef void (*DestroyFunc)(void *item);

/* pointer to function that visits an item */
typedef void (*VisitFunc)(void *item, int key);

/* pointer to function that gets the key */
typedef void *(*GetKey)(void *item);

/* pointer to function that prints an item */
typedef void (*PrintItem)(void *item);

#endif