#include <stdio.h>
#include <time.h>

#include <idletime_x11.h>

#include <curl/curl.h>

#include "config.h"

//#ifdef __unix__
#include <unistd.h>
//#endif

//Время простоя, которое было при предыдущем опросе
static int last_time;
//Логин юзера в системе
static char *username = 0;
//IP машины
static char *ip_addr = 0;
//Указатель на curl
CURL *curl_handle;

//char *default_url = "http://yandex.ru/";

//char *default_url = "http://itimer.elmarket.local.ru/index.php/set/index?var=set_log&login=ursolov.vs&state=1&ip=192.168.2.10&localtime=2012-01-22%2014:14:02&debug=json";

struct MemoryStruct {
    char *memory;
    size_t size;
};

size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    register int realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

char *getUsername(){
    char *buf = malloc(255* sizeof(char));
    FILE *name;
    name = popen("whoami", "r");
    fgets(buf, 255, name);
    pclose(name);
    //Убрать \n с конца строки
    char *buf2 = malloc((strlen(buf)-1)* sizeof(char));
    buf2 = strncpy(buf2, buf, (strlen(buf)-1)* sizeof(char));
    return buf2;
}

char *getTime(){
    char *buf = malloc (50 * sizeof (char));;
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
   // sprintf ( buf, "%i-%i-%i %i:%i:%i", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );

    strftime(buf, 50, "%Y-%m-%d %H:%M:%S", timeinfo);
    return buf;
}

char *getIp(){
    char *buf = malloc(40* sizeof(char));
    FILE *name;
    name = popen("ifconfig eth | grep -P '(?<=inet )192\\.168\\.\\d{1,3}\\.\\d{1,3}' -o", "r");
    //name = popen("ifconfig eth | grep -P '(?<=inet )192\\.168\\.\\d*\\.\\d*' -o", "r");
   // name = popen("ifconfig eth|grep '192'", "r");
    fgets(buf, 40, name);
    //Убрать \n с конца строки
    char *buf2 = malloc((strlen(buf)-1)* sizeof(char));
    buf2 = strncpy(buf2, buf, (strlen(buf)-1)* sizeof(char));
    pclose(name);
    return buf2;
}

int main(void)
{

    if (!idletime_init()) {
        printf('Error in initializationg screensaver extension');
        return 1;
    }


    username = getUsername();
    ip_addr = getIp();
    printf("Имя пользователя: %s", username);
    printf("IP адрес машины: %s", ip_addr);
    struct MemoryStruct chunk;
    chunk.memory=NULL;
    chunk.size = 0;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);



    //    curl_easy_perform(curl_handle);


//    printf(chunk.memory);


    unsigned int idle_time;
    int idle_time_rounded;
    while (1){
        idle_time = idletime_get();
        idle_time_rounded = (idle_time+500)/1000;
        //printf("%i\n", last_time);
        //printf("%i\n", idle_time_rounded);

        if ( last_time > idle_time_rounded ) {
            char *url = malloc(500 * sizeof(char));
            sprintf (url, url_mask, username, ip_addr, getTime());
            printf("%s\n", url);
            curl_easy_setopt(curl_handle, CURLOPT_URL, url);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        }
        last_time = idle_time_rounded;
        //printf("%u\n", idletime_get());
        fflush(stdout);
        sleep(1);
    }

    curl_easy_cleanup(curl_handle);

    idletime_free();
    return 0;
}

