#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HTTP_GET 0
#define HTTP_POST 1
#define HTTP_PUT 2
#define HTTP_DELETE 3

#define REGEX_HTTP_GET "GET //(.*) HTTP/1.[0|1]"
#define REGEX_HTTP_POST "POST //(.*) HTTP/1.[0|1]"
#define REGEX_HTTP_PUT "PUT //(.*) HTTP/1.[0|1]"
#define REGEX_HTTP_DELETE "DELETE //(.*) HTTP/1.[0|1]"

bool is_http_get(char *request);

bool is_http_post(char *request);

bool is_http_put(char *request);

bool is_http_delete(char *request);

bool evaluate_regex(char *request, char *regex);

char * get_http_method(char *request);

#endif