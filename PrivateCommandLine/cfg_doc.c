#include "cfg_doc.h"

//split the buffer
char** cfg_split(char* title, int num_params, char* buff){
    int i=0;
    const char s1[2] = "{",s2[2]=":",s3[2]=",";
    char* token;
    char* start=(char*)calloc(strlen(strstr(buff,title)),sizeof(char));
    start = strstr(buff,title);
    char* end = (char*)calloc(strlen(strchr(start,'}')),sizeof(char));
    end = strchr(start,'}');
    char* obj_str=(char*)calloc(end-start,sizeof(char));
    obj_str = start;
    char** valuesArr=(char**)calloc(num_params,sizeof(char*));
    token = strtok(obj_str,s1);
    while( token != NULL &&i<num_params) {
        char* value=strchr(token,':');
        value++;
        token = strtok(NULL, s2);
        token = strtok(NULL, s3);
        valuesArr[i++]=token;
    }
    return valuesArr;
}

void read_file(CFG* config){
    struct stat st;
    stat(JSONFILEPATH, &st);
    char *buff=(char*)malloc(st.st_size*sizeof (char)),*temp_buff;
    config->json_file=fopen(JSONFILEPATH,"r");
    if(config->json_file==NULL){
        printf("error");;
        return;
    }
    fread(buff,sizeof(char),st.st_size,config->json_file);
    fclose(config->json_file);
    temp_buff=strdup(buff);
    read_record(config->widget,temp_buff);
    temp_buff=strdup(buff);
    read_snapshot(config->widget,temp_buff);
    temp_buff=strdup(buff);
    read_streaming(config->widget,temp_buff);
    read_gpio(config->widget,buff);
    free(buff);
}

char* create_buffer(WIDGET* widget){
    int buffer_len;
    char* record_buffer=create_buffer_record(widget->record);
    char* snapshot_buffer=create_buffer_snapshot(widget->snapshot);
    char* streaming_buffer=create_buffer_streaming(widget->streaming);
    char* gpio_buffer=create_buffer_gpio(widget->gpio);
    buffer_len=strlen(record_buffer)+strlen(snapshot_buffer)+strlen(streaming_buffer)+strlen(gpio_buffer);
    char* buffer=(char*)calloc(buffer_len,sizeof (char));
    if(!buffer)exit(1);
    strcat(buffer,record_buffer);
    strcat(buffer,snapshot_buffer);
    strcat(buffer,streaming_buffer);
    strcat(buffer,gpio_buffer);
    return buffer;
}

void write_file(CFG* config){
    char* buffer=create_buffer(config->widget);
    config->json_file=fopen(JSONFILEPATH, "w+");
    fwrite(buffer,strlen(buffer),sizeof(char),config->json_file);
    fclose(config->json_file);
    free(buffer);
}
//objects
void write_record(PWIDGET widget, record_t* record){
    widget->record = record;
}

void write_snapshot(PWIDGET widget, snapshot_t* snapshot){
    widget->snapshot = snapshot;
}

void write_streaming(PWIDGET widget, streaming_t* streaming){
    widget->streaming = streaming;
}

void write_gpio(PWIDGET widget, gpio_t* gpio){
    widget->gpio = gpio;
}

void read_record(PWIDGET widget, char* buffer){
    char** arr_params=cfg_split("record",5,buffer);
    widget->record->file_name=strdup(arr_params[0]);
    widget->record->codec=atoi(arr_params[1]);
    widget->record->width=atoi(arr_params[2]);
    widget->record->height=atoi(arr_params[3]);
    widget->record->fps=atoi(arr_params[4]);
}

void read_snapshot(PWIDGET widget, char* buffer){
    char** arr_params=cfg_split("snapshot",4,buffer);
    widget->snapshot->file_name=strdup(arr_params[0]);
    widget->snapshot->width=atoi(arr_params[1]);
    widget->snapshot->height=atoi(arr_params[2]);
    widget->snapshot->type=atoi(arr_params[3]);
}

void read_streaming(PWIDGET widget, char* buffer){
    char** arr_params=cfg_split("streaming",2,buffer);
    widget->streaming->ip=atoi(arr_params[0]);
    widget->streaming->port=atoi(arr_params[1]);
}

void read_gpio(PWIDGET widget, char* buffer){
    char** arr_params=cfg_split("gpio",5,buffer);
    widget->gpio->num=atoi(arr_params[0]);
    widget->gpio->dir=atoi(arr_params[1]);
    widget->gpio->value=atoi(arr_params[2]);
    widget->gpio->is_blink=atoi(arr_params[3]);
    widget->gpio->opertion=atoi(arr_params[4]);
}

char* create_buffer_record(record_t* record){
    char* record_buffer=(char*)calloc(DECLARATIONSIZE,sizeof(char));
    sprintf(record_buffer,"\"record\": {\n\t\"file_name\":\"%s\",\n\t\"codec\":%d,\n\t\"width\":%d,\n\t\"height\":%d,\n\t\"fps\":%d,\n}",record->file_name,record->codec,record->width,record->height,record->fps);
    return record_buffer;
}

char* create_buffer_snapshot(snapshot_t* snapshot){
    char* snapshot_buffer=(char*)calloc(DECLARATIONSIZE,sizeof(char));
    sprintf(snapshot_buffer,"\"snapshot\": {\n\t\"file_name\":\"%s\",\n\t\"width\":%d,\n\t\"height\":%d,\n\t\"type\":%d,\n}",snapshot->file_name,snapshot->width,snapshot->height,snapshot->type);
    return snapshot_buffer;
}

char* create_buffer_streaming(streaming_t* streaming){
    char* streaming_buffer=(char*)calloc(DECLARATIONSIZE,sizeof(char));
    sprintf(streaming_buffer,"\"streaming\": {\n\t\"ip\":%d,\n\t\"port\":%d,\n}",streaming->ip,streaming->port);
    return streaming_buffer;
}

char* create_buffer_gpio(gpio_t* gpio){
    char* gpio_buffer=(char*)calloc(DECLARATIONSIZE,sizeof(char));
    sprintf(gpio_buffer,"\"gpio\": {\n\t\"num\":%d,\n\t\"dir\":%d,\n\t\"value\":%d,\n\t\"is_blink\":%d,\n\t\"operation\":%d,\n}",gpio->num,gpio->dir,gpio->value,gpio->is_blink,gpio->opertion);
    return gpio_buffer;
}
