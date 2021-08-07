#include "specific.h"
#include "../assoc.h"

/* Source: http://www.cse.yorku.ca/~oz/hash.html */
int hash1(assoc* a, void* key);
/* Source: https://www.geeksforgeeks.org/double-hashing/ */
int hash2(assoc* a, void *key);
/* Finds next prime after j */
int next_prime(int j);
/* Resizes table and re-hashes all values */
void resize(assoc** pp, int sz);
/* Creates new table with more capacity */
assoc* resize_init(int keysize, int sz);
/* Probes until it reaches empty space or same key */
void probe(assoc* a, int* index, int* prob, void* key);
/* Checks if a given key exists in a given index */
bool exists(assoc* a, int* index, void* key);
/* (For testing purposes) */
void print_table(assoc* a);
void test();

assoc* assoc_init(int keysize)
{
   static int g = 0;

   if (g == 0)
   {
      g = 1;
      test();
   }

   if (keysize >= 0)
   {
      assoc* a;
      a = ncalloc(1, sizeof(assoc));
      a->capacity = INITCAPACITY;
      a->array = (kdPair*) ncalloc(a->capacity, sizeof(kdPair));
      a->size = 0;
      a->keysize = keysize;
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
      int* prob;
      int h, p;
      unsigned int threshold = LOADFACTOR * (*a)->capacity;

      h = hash1(*a, key);
      p = hash2(*a, key);
      index = &h;
      prob = &p;

      probe(*a, index, prob, key);

      if (exists(*a, index, key))
      {
         (*a)->array[*index].d = data;
      }
      else
      {
         (*a)->array[*index].k = key;
         (*a)->array[*index].d = data;
         ((*a)->size)++;
      }

      if ((*a)->size >= threshold)
      {
         resize(a, next_prime((*a)->capacity * SCALEFACTOR));
      }
   }
   else
   {
      on_error("You have tried to pass a NULL pointer to 'assoc_insert'!");
   }
}

void* assoc_lookup(assoc* a, void* key)
{
   if ((a) && (key))
   {
      int* index = NULL;
      int* prob = NULL;
      int h, p;

      h = (hash1(a, key));
      p = hash2(a, key);
      index = &h;
      prob = &p;

      probe(a, index, prob, key);
      return a->array[*index].d;
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
      on_error("You have tried to pass a NULL pointer to 'assoc_count'!");
   }
   return 0;
}

void assoc_free(assoc* a)
{
   if (a)
   {
      free(a->array);
      free(a);
   }
   else
   {
      on_error("You have tried to pass a NULL pointer to 'assoc_free'!");
   }
}

void probe(assoc* a, int* index, int* prob, void* key)
{
   int i = 1;

   if (a->keysize == 0)
   {
      while (a->array[*index].k != NULL && strcmp(a->array[*index].k, key) != 0)
      {
         *index += (*prob * i);
         *index %= a->capacity;
         i++;
      }
   }
   else
   {
      while (a->array[*index].k != NULL && memcmp(a->array[*index].k, key, sizeof(a->keysize)) != 0)
      {
         *index += (*prob * i);
         *index %= a->capacity;
         i++;
      }
   }
}

bool exists(assoc* a, int* index, void* key)
{
   if (a->array[*index].k != NULL)
   {
      if (a->keysize == 0 && strcmp(a->array[*index].k, key) == 0)
      {
         return true;
      }
      else if (memcmp(a->array[*index].k, key, sizeof(a->keysize)) == 0)
      {
         return true;
      }
   }
   return false;
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
   }
   assoc_free(*pp);
   *pp = new;
}

assoc* resize_init(int keysize, int sz)
{
   assoc* a;
   a = ncalloc(1, sizeof(assoc));
   a->capacity = sz;
   a->array = (kdPair*) ncalloc(a->capacity, sizeof(kdPair));
   a->size = 0;
   a->keysize = keysize;
   return a;
}

int hash1(assoc* a, void* key)
{
   unsigned long hash = HASHCONST1;
   int i = 0;
   unsigned char* k;
   k = (unsigned char*)key;

   if (a->keysize == 0)
   {
      while (i < (int)strlen(key))
      {
         hash = HASHCONST2 * hash ^ *(k + i);
         i++;
      }
   }
   else
   {
      while (i < (int)sizeof(a->keysize))
      {
         hash = HASHCONST2 * hash ^ *(k + i);
         i++;
      }
   }
   return (int)(hash % a->capacity);
}

int hash2(assoc* a, void *key)
{
   return HASHCONST3 - (hash1(a, key) % HASHCONST3);
}

int next_prime(int j)
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

void print_table(assoc* a)
{
   int i;
   for (i = 0; i < 500; i++)
   {
      if (a->array[i].k == NULL)
      {
         printf("\t%d\t---\n", i);
      }
      else
      {
         printf("\t%d\t%d\n", i, *(int*)a->array[i].k);
      }
   }
   printf("capacity: %d\n", a->capacity);
}

void test()
{
   assoc* a = NULL;
   assoc* b = NULL;
   assoc* c = NULL;
   assoc* d = NULL;
   assoc* e = NULL;
   assoc* f = NULL;
   assoc* g = NULL;
   assoc* s = NULL;

   int i, j;
   int k = -87;
   int p = -965;
   int q = -9638;
   float fl;
   double dd;
   int arr[100];
   long l;
   char* gabriel = "gabriel";
   char* james = "james";
   char* william = "william";
   char* carlos = "carlos";

   int* p1 = NULL;
   float* p2 = NULL;
   double* p3 = NULL;
   long* p5 = NULL;

   a = assoc_init(sizeof(int));
   a->capacity = 211;

   b = assoc_init(sizeof(float));
   b->capacity = 211;

   c = assoc_init(sizeof(double));
   c->capacity = 211;

   d = assoc_init(sizeof(short));
   d->capacity = 211;

   e = assoc_init(sizeof(long));
   e->capacity = 211;

   f = assoc_init(sizeof(char));
   f->capacity = 211;

   g = assoc_init(sizeof(arr));
   g->capacity = 211;

   s = assoc_init(0);

   /* Testing "hash1" and "hash" 2 functions with various data types*/
   srand(time(NULL));

   /* Testing hash functions with integers */
   for (i = 0; i < 100; i++)
   {
      j = rand() % 211;
      p1 = &j;
      arr[i] = hash1(a, p1);
      assert(hash2(a, &arr[i]) != 0);
      assert(arr[i] >= 0 && arr[i] <= 211);
   }

      p1 = &k;
      assert(hash1(a, p1) >= 0 && hash1(a, p1) <= 211);
      p1 = &p;
      assert(hash1(a, p1) >= 0 && hash1(a, p1) <= 211);
      p1 = &q;
      assert(hash1(a, p1) >= 0 && hash1(a, p1) <= 211);

   /* Testing hash functions with floats */
   for (i = 0; i < 100; i++)
   {
      fl = (float)rand() / (float)(RAND_MAX / 211);
      p2 = &fl;
      arr[i] = hash1(b, p2);
      assert(hash2(a, &arr[i]) != 0);
      assert(arr[i] >= 0 && arr[i] <= 211);
   }


   /* Testing hash functions with doubles */
   for (i = 0; i < 100; i++)
   {
      dd = (double)rand() / (double)(RAND_MAX / 211);
      p3 = &dd;
      arr[i] = hash1(c, p3);
      assert(hash2(a, &arr[i]) != 0);
      assert(arr[i] >= 0 && arr[i] <= 211);
   }

   /* Testing hash functions with longs */
   for (i = 0; i < 100; i++)
   {
      l = (long)rand() / (long)(RAND_MAX / 100000000);
      p5 = &l;
      arr[i] = hash1(e, p5);
      assert(hash2(a, &arr[i]) != 0);
      assert(arr[i] >= 0 && arr[i] <= 211);
   }

   /* Testing hash functions with strings */

   assert(hash1(s, gabriel) >= 0 && hash1(s, gabriel) <= 17);
   assert(hash1(s, james) >= 0 && hash1(s, james) <= 17);
   assert(hash1(s, william) >= 0 && hash1(s, william) <= 17);

   /* Testing 'assoc_insert', 'assoc_lookup', and assoc_count functions*/

   assoc_insert(&s, gabriel, "chu");
   assoc_insert(&s, james, "smith");
   assoc_insert(&s, william, "brown");

   assert(strcmp(assoc_lookup(s, gabriel), "chu") == 0);
   assert(strcmp(assoc_lookup(s, james), "smith") == 0);
   assert(strcmp(assoc_lookup(s, william), "brown") == 0);
   assert(assoc_count(s) == 3);

   assoc_insert(&s, james, "williams");
   assoc_insert(&s, carlos, "rodriguez");
   assert(strcmp(assoc_lookup(s, james), "smith") != 0);
   assert(strcmp(assoc_lookup(s, james), "williams") == 0);
   assert(assoc_count(s) == 4);

   /* Testing "next_prime" function */
   assert(next_prime(14) == 17);
   assert(next_prime(434) == 439);
   assert(next_prime(9968) == 9973);
   assert(next_prime(56994) == 56999);
   assert(next_prime(89592) == 89597);
   assert(next_prime(109884) == 109891);
   assert(next_prime(158262) == 158269);
   assert(next_prime(200515) == 200569);
   assert(next_prime(242790) == 242797);

   free(a);
   free(b);
   free(c);
   free(d);
   free(e);
   free(f);
   free(g);
}
