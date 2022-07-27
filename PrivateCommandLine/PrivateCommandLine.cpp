#include<stdio.h>
#include<stdlib.h>
#include"PrivateCommandLine.h"
#include<ctype.h>
#include<string.h>
#include"gaz_cam_lib.h"
#include"command_handler.h"

//extern gas_api *p_gaz;
int GAS_API_GET_DLL_VERSION(int numParams,char** params)
{

    return 1;
}
int GAS_API_START_RECORD(int numParams,char** params)
{
    return 1;
}
int GAS_API_STOP_RECORD(int numParams,char** params)
{
    return 1;
}
int GAS_API_START_STREAMING(int numParams,char** params)
{
    return 1;
}
int GAS_API_STOP_STREAMING(int numParams,char** params)
{
    return 1;

}
int GAS_API_DO_SNAPSHOT(int numParams,char** params)
{
    return 1;

}
int GAS_API_GET_VIDEO_STATISTIC(int numParams,char** params)
{
    return 1;

}
int GAS_API_GET_STATUS (int numParams,char** params)
{
 return 1;
}

//void start_record(int num)
//{
//     printf("num from start_record:%d  \n",num);
//}
//void stop_record(int num)
//{
//    printf(" num from stop_record: %d\n",num);
//}
//void print_all_records(char* str1,char* str2)
//{
//    printf("str1 from print_all_records:%s  \n",str1);
//    printf("str2 from print_all_records:%s  \n",str2);
//}
//void save_record(char* str)
//{
//    printf("str from save_record:%s  \n",str);
//}
//void delete_record(char* str)
//{
//    printf("str from delete_record:%s  \n",str);
//}
//void start_record_cli(int numParams,char**params)
//{
//    if(numParams!=1||!atoi(params[0]))
//        printf("error. faild in start_rcord function ");
//    start_record(atoi(params[0]));
//}
//void stop_record_cli(int numParams,char**params)
//{
//    if(numParams!=1||!atoi(params[0]))
//        printf("error. faild in stop_rcord function ");
//    else
//        stop_record(atoi(params[0]));
//}
//void print_all_records_cli(int numParams,char**params)
//{
//    if(numParams!=3)
//        printf("error. faild in print_all_rcords function ");
//    else
//        print_all_records(params[0],params[1]);
//}
//void save_record_cli(int numParams,char**params)
//{
//    if(numParams!=1)
//        printf("error. faild in save_record function ");
//    else
//        save_record(params[0]);
//}
//void delete_record_cli(int numParams,char**params)
//{
//    if(numParams!=1)
//        printf("error. faild in delete_record function ");
//    else
//        save_record(params[0]);
//}
