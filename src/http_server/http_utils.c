
#include <stdio.h>
#include <regex.h>

#include "http_utils.h"




bool is_http_get(char *request){
    return evaluate_regex(request, REGEX_HTTP_GET );
}

bool is_http_post(char *request){
    return evaluate_regex(request, REGEX_HTTP_POST );
}   

bool is_http_put(char *request){
    return evaluate_regex(request, REGEX_HTTP_PUT );
}   

bool is_http_delete(char *request){
    return evaluate_regex(request, REGEX_HTTP_DELETE );
}


bool get_http_header(const char *request, char **header){
    bool success = false;
    int len = 0;
    char *pFind = strstr(request, DELIMITER_HTTP_HEADER);
    if (pFind != NULL) {
        len = pFind - request ;
        *header = (char *) malloc(len+1);
        memset(*header, 0, len+1);
        memcpy(*header, request, len);
        success = true;
    }
    return success;
}

bool get_http_body(const char *request, char **body) {
    bool success = false;
    char *pFind = strstr(request, DELIMITER_HTTP_HEADER);
    // segun el protocolo vienen los headers y el body separados por un \r\n\r\n
    if (pFind != NULL) {
        *body = (char *) malloc(strlen(pFind));
        memset(*body, 0, strlen(pFind));
        // le sacamos los 4 bytes del \r\n\r\n
        memcpy(*body, &pFind[4], strlen(pFind-4));
        success = true;
    }
    return success;
}

bool get_http_context(const char *request, const char *method, char **path){
    bool success = false;
    char find[CHAR_LEN];
    int pos_ini = 0, pos_end = 0, len = 0;
    memset(find, 0, CHAR_LEN);
    strcpy(find, method);
    strcat(find, " //");
    char *pFind = strstr(request, find);
    if (pFind != NULL) {
        pos_ini = pFind - request;
        pos_ini += strlen(find);
        char *pHttp = strstr(request, "HTTP");
        if (pHttp != NULL){
            pos_end = pHttp - request;
            if (pos_end - pos_ini > 0) {
                len = pos_end - pos_ini;
                *path = (char *) malloc(len+1);
                memset(*path, 0, len+1);
                memcpy(*path, &request[pos_ini], len);
                success = true;
            }
        }
    }
    return success;
}

bool evaluate_regex(char *request, char * regex_text){
    bool found = false;
    regex_t regex; 
    int ret = regcomp(&regex, regex_text, REG_EXTENDED | REG_NOSUB | REG_NEWLINE);
    if (ret) {
        fprintf(stderr, "Error al compilar regex.\n");
        return found;
    }else {
        ret = regexec(&regex, request, 0, NULL, 0);
        if (ret == 0) found = true;
        else if (ret == REG_NOMATCH) found = false;
        else found = false;
        
    }
    regfree(&regex);

    return found;
}

char * get_http_method(char *request){
    if( is_http_get(request) ) return "GET";
    if( is_http_post(request) ) return "POST";
    if( is_http_put(request) ) return "PUT";
    if( is_http_delete(request) ) return "DELETE";
    return "DESCONOCIDO";
}
