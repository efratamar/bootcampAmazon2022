#ifndef HASHTABLE_H
#define HASHTABLE_H
unsigned long hash_function(char*);
typedef struct Ht_item Ht_item;
struct value{
    int (*func)(int ,char**);
    char*help;
};

// Define the Hash Table Item here
struct Ht_item {
    char* key;
    struct value value;
};

typedef struct HashTable HashTable;

// Define the Hash Table here
struct HashTable {
    // Contains an array of pointers
    // to items
    Ht_item** items;
    int size;
    int count;
};
Ht_item* create_item(char*, char*);
HashTable* create_table(int);
void free_item(Ht_item*);
void free_table(HashTable*);
void handle_collision(HashTable*, unsigned long, Ht_item*);
void ht_insert(HashTable*, char*,int(*func)(int,char**), char*);
struct value ht_search(HashTable* table, char* key);
void print_search(HashTable*, char*);
void print_table(HashTable*);

#endif // HASHTABLE_H
