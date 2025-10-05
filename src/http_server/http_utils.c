
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
