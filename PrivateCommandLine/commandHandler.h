#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H
#include "PrivateCommandLine.h"
#include "gaz_cam_lib.h"
#include "HashTable.h"
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#define MAXPARAMETERS 100
#define CAPACITY 500 // Size of the Hash Table


void init_hash_table(HashTable*);
void decoderfunction(HashTable*,char**);
char** split(char*);
gas_api* load_library();
void handle_sigint(int);

#endif // COMMAND_HANDLER_H
