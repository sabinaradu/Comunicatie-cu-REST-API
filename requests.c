#include <stdlib.h>     
#include <stdio.h>
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <string.h>


char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                             char *cookies, char *auth)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    if (auth != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth);
        compute_message(message, line);
    }

    if (cookies != NULL) {
       sprintf(line, "%s", cookies);
       compute_message(message, line);
    }

    compute_message(message, "");
    sprintf(line, "%s", body_data);
    compute_message(message, line);

    return message;
}


char *compute_delete_request(char *host, char *url, char* content_type,
                             char *auth)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    if (auth != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth);
        compute_message(message, line);
    }

    compute_message(message, "");

    return message;
}


char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char *auth)
{
	char *line = calloc(LINELEN, sizeof(char));
    char *message = calloc(BUFLEN, sizeof(char));

    if (query_params == NULL) {
        sprintf(line, "GET %s HTTP/1.1", url);
    } else {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (auth != NULL) {
        sprintf(line, "Authorization: Bearer %s", auth);
        compute_message(message, line);
    }

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}