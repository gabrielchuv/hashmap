/* Works only for strings */

#include "specific.h"
#include "../assoc.h"
#include <math.h>

int hash1(assoc* a, void* key);
int nextPrime(int j);
void resize(assoc** pp, int sz);
bool exists(assoc* a, int* index, void* key);
assoc* resize_init(int keysize, int sz);
void printTable(assoc* a);
int hash2(assoc* a, void* key);
void insert(assoc* x, int* index, void* key, void* data);
void insert2(assoc* x, int* index, void* key, void* data);
unsigned int assoc_count(assoc* a);
void* assoc_lookup(assoc* a, void* key);
void recur(assoc* x, int* index, void* key, void* data, int* cnt);

assoc* assoc_init(int keysize)
{
   if (keysize >= 0)
   {
      assoc* a = NULL;
      assoc* b = NULL;
      a = ncalloc(1, sizeof(assoc));
      b = ncalloc(1, sizeof(assoc));

      a->capacity = INITCAPACITY;
      a->array = (kdPair*) ncalloc(a->capacity, sizeof(kdPair));
      a->size = 0;
      a->keysize = keysize;
      a->id = 0;
      a->otherTable = b;

      b->capacity = INITCAPACITY;
      b->array = (kdPair*) ncalloc(a->capacity, sizeof(kdPair));
      b->size = 0;
      b->keysize = keysize;
      b->id = 1;
      b->otherTable = a;

      return a;
   }
   on_error("keysize must be a positive int!");
   return NULL;
}

void assoc_insert(assoc** a, void* key, void* data)
{
   if((a) && (*a) && (key))
   {
      int* index = NULL;
      int h;
      int* cnt = NULL;
      int c = 0;
      unsigned int threshold = LOADFACTOR * (*a)->capacity;

      h = hash1(*a, key);
      index = &h;

      cnt = &c;
      recur(*a, index, key, data, cnt);
      ((*a)->size)++;

      if  (assoc_count(*a) - ((*a)->otherTable->size) >= threshold || (*a)->otherTable->size >= threshold)
      {
         resize(a, nextPrime((*a)->capacity * SCALEFACTOR));
      }
   }
   else
   {
      on_error("You have tried to pass a NULL pointer to 'assoc_insert'!");
   }
}

void recur(assoc* x, int* index, void* key, void* data, int* cnt)
{
   int h1, h2;

   if (x->array[*index].k == NULL)
   {
      insert(x, index, key, data);
   }
   else
   {
      if (x->id == 0)
      {
         h2 = hash2(x, x->array[*index].k);
         (*cnt)++;
         recur(x->otherTable, &h2, x->array[*index].k, data, cnt);
         insert(x, index, key, data);
      }
      else
      {
         h1 = hash1(x, x->array[*index].k);
         (*cnt)++;
         recur(x->otherTable, &h1, x->array[*index].k, data, cnt);
         insert(x, index, key, data);
      }
   }
}

void insert(assoc* x, int* index, void* key, void* data)
{
   x->array[*index].k = key;
   x->array[*index].d = data;
}

int hash1(assoc* a, void* key)
{
   unsigned long hash = 5381;
   int i = 0;
   unsigned char *k;
   k = (unsigned char*)key;

   if (a->keysize == 0)
   {
      while (i < (int)strlen(key))
      {
         hash = 33 * hash ^ *(k + i);
         i++;
      }
   }
   else
   {
      while (i < (int)sizeof(a->keysize))
      {
         hash = 33 * hash ^ *(k + i);
         i++;
      }
   }
   return (int)(hash % a->capacity);
}

int hash2(assoc* a, void* key)
{
   unsigned long hash = 0;
   int i = 0;
   unsigned char *k;
   k = (unsigned char*)key;

    if (a->keysize == 0)
    {
       while (i < (int)strlen(key))
       {
          hash = *(k + i) + (hash << 6) + (hash << 16) - hash;
          i++;
       }
    }
    else
    {
      while (i < (int)sizeof(a->keysize))
      {
         hash = *(k + i) + (hash << 6) + (hash << 16) - hash;
         i++;
      }
    }
   return hash % a->capacity;
}

void* assoc_lookup(assoc* a, void* key)
{
   if ((a) && (key))
   {
      int *index = NULL;
      int h;

      h = (hash1(a, key));
      index = &h;

      if (a->array[*index].k != NULL)
      {
         if (memcmp(a->array[*index].k, key, sizeof(a->keysize)) == 0)
         {
            return a->array[*index].d;
         }

         h = (hash2(a, key));
         index = &h;
      }

      else if (a->otherTable->array[*index].k != NULL)
      {
         if (memcmp(a->otherTable->array[*index].k, key, sizeof(a->keysize)) == 0)
         {
            return a->otherTable->array[*index].d;
         }
      }
      else
      {
         return NULL;
      }
   }
   else
   {
      on_error("You have tried to pass a NULL pointer to 'assoc_lookup'!");
   }
   return NULL;
}

unsigned int assoc_count(assoc* a)
{
   if(a)
   {
      return a->size;
   }
   else
   {
      on_error("You have tried to pass a NULL pointer to 'assoc_count'!"); /* change? */
   }
   return 0;
}


int nextPrime(int j)
{
   int i = 2;
   while (i != j)
   {
      i = 2;
      while (j % i != 0)
      {
         i++;
      }
      if (i != j)
      {
         j++;
      }
   }
   return j;
}

void assoc_free(assoc* a)
{
   if (a)
   {
      free(a->otherTable->array);
      free(a->otherTable);
      free(a->array);
      free(a);
   }
   else
   {
      on_error("You have tried to pass a NULL pointer to 'assoc_free'!");
   }
}

void resize(assoc** pp, int sz)
{
   assoc* new = NULL;
   int i;

   new = resize_init((*pp)->keysize, sz);

   for (i = 0; i < (int)(*pp)->capacity; i++)
   {
      if ((*pp)->array[i].k != NULL)
      {
         assoc_insert(&new, (*pp)->array[i].k, (*pp)->array[i].d);
      }
      if ((*pp)->otherTable->array[i].k != NULL)
      {
         assoc_insert(&new, (*pp)->otherTable->array[i].k, (*pp)->array[i].d);
      }
   }
   assoc_free(*pp);
   *pp = new;
}

assoc* resize_init(int keysize, int sz)
{
   assoc* a = NULL;
   assoc* b = NULL;
   a = ncalloc(1, sizeof(assoc));
   b = ncalloc(1, sizeof(assoc));

   a->capacity = sz;
   a->array = (kdPair*) ncalloc(a->capacity, sizeof(kdPair));
   a->size = 0;
   a->keysize = keysize;
   a->id = 0;
   a->otherTable = b;

   b->capacity = sz;
   b->array = (kdPair*) ncalloc(b->capacity, sizeof(kdPair));
   b->size = 0;
   b->keysize = keysize;
   b->id = 1;
   b->otherTable = a;

   return a;
}

void printTable(assoc* a)
{
   int i;
   for (i = 0; i < (int)a->capacity; i++)
   {
      if (a->array[i].k == NULL)
      {
         printf("\t%d\t---\n", i);
      }
      else
      {
         printf("\t%d\t%s\n", i, a->array[i].k);
      }
   }
   printf("capacity: %d\n", a->capacity);
}
