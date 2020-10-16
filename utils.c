
#include "utils.h"

char* find_event_file(char* ev){

    FILE* fp = NULL;
    char* line = NULL;
    size_t size = 0;
    int event = 0;
    int length = 0;
    
    char* path = "/dev/input/event";
    char* filename;
    char* event_file_number;

    fp = fopen("/proc/bus/input/devices", "r");
    length = getline(&line, &size, fp);
    while(length != -1){
        if(strncmp(line, "B: EV=", 6) == 0){
            if(strncmp(line, ev, strlen(ev)) == 0){
                event_file_number = malloc(sizeof(char) * 10);
                sprintf(event_file_number, "%d", event);
                int size = sizeof(char) * (strlen(path) + strlen(event_file_number)) + 1;
                filename = malloc(size);
                strcpy(filename, path);
                strcat(filename, event_file_number);
                free(event_file_number);
                free(line);
                fclose(fp);
                return filename;
            }
            event++;
        }
        length = getline(&line, &size, fp);
    }

    free(line);
    fclose(fp);
    return NULL;
}
