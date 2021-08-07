#define INITCAPACITY 17 
#define SCALEFACTOR 2
#define LOADFACTOR 0.6
#define HASHCONST1 5381
#define HASHCONST2 33
#define HASHCONST3 7

typedef enum bool {false, true} bool;

typedef struct kdPair
{
   void* k;
   void* d;
}kdPair;

typedef struct assoc
{
   kdPair* array;
   unsigned int size;
   unsigned int capacity;
   unsigned int keysize;
}assoc;
