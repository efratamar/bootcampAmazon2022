#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include "commandHandler.h"
#include "HashTable.h"
#include "PrivateCommandLine.h"
#define MAXPARAMETERS 100
#define CAPACITY 500 // Size of the Hash Table

gas_api* p_gaz;
p_handler handler;

int main()
{
    signal(SIGINT, handle_sigint);//CTRL+C
    p_gaz=load_library();
    handler=p_gaz->init_dll();
    record_t record;
    snapshot_t snapshot;
    snapshot.file_name=BMPFILEPATH;
    p_gaz->start_record(handler,record);
    //p_gaz->do_snapshot(handler,snapshot);
    while (1) {
        usleep(WAIT);
           PRINTF_DBG("main loop\n");
    }
    return 0;
}
