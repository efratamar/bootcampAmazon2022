#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H
#include<string.h>
#include"PrivateCommandLine.h"
#include<stdio.h>
#include<HashTable.h>
#include <dlfcn.h>
#include "gaz_cam_lib.h"
#include<stdlib.h>
#include<assert.h>

void init_hash_table(HashTable*);
void decoderfunction(HashTable*,char**);
char** split(char*);
gas_api* load_library();

#endif // COMMAND_HANDLER_H
