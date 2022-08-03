#ifndef PRIVATECOMMANDLINE_H
#define PRIVATECOMMANDLINE_H

//void start_record(int);
//void stop_record(int);
//void print_all_records(char*,char*);
//void save_record(char*);
//void delete_record(char*);
//void start_record_cli(int,char**);
//void stop_record_cli(int,char**);
//void print_all_records_cli(int,char**);
//void save_record_cli(int,char**);
//void delete_record_cli(int,char**);
int GAS_API_GET_DLL_VERSION_CLI(int,char**);
int GAS_API_START_RECORD_CLI(int,char**);
int GAS_API_STOP_RECORD_CLI(int,char**);
int GAS_API_START_STREAMING_CLI(int,char**);
int GAS_API_STOP_STREAMING_CLI(int,char**);
int GAS_API_DO_SNAPSHOT_CLI(int,char**);
int GAS_API_GET_VIDEO_STATISTIC_CLI(int,char**);
int GAS_API_GET_STATUS_CLI (int,char**);

#endif // PRIVATECOMMANDLINE_H
