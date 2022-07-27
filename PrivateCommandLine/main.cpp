#include <QCoreApplication>
#include<string.h>
#include"PrivateCommandLine.h"
#include<stdio.h>
#include<stdlib.h>
#include"HashTable.h"
#define MAXPARAMETERS 100
#define CAPACITY 500 // Size of the Hash Table
#include <dlfcn.h>
#include"command_handler.h"
#include"queue.h"
gas_api* p_gaz;

int main()
{
    p_gaz=load_library();
    handler_t* handler=p_gaz->init_dll();
    record_t record;
    p_gaz->start_record(handler,record);
    HashTable* ht = create_table(CAPACITY);
    init_hash_table(ht);
    char**splitArray=(char**)malloc(sizeof(char*));
    char path[MAXPARAMETERS];
    while (1) {
        fgets(path,MAXPARAMETERS,stdin);
        splitArray=split(path);
        decoderfunction(ht,splitArray);
    }
    free_table(ht);
    return 0;
}
