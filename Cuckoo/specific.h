#define INITCAPACITY 17 
#define SCALEFACTOR 2
#define LOADFACTOR 0.1
#define TABLES 2

typedef enum bool {false, true} bool;

typedef struct kdPair
{
   void *k;
   void *d;
}kdPair;

typedef struct assoc
{
   kdPair *array;
   unsigned int size;
   unsigned int capacity;
   unsigned int keysize;
   int id;
   struct assoc *otherTable;
}assoc;
