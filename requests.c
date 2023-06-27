#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *tok)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    int offset;

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    memset(line, 0, LINELEN);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    memset(line, 0, LINELEN);

    // Step 3 (optional): add token
    if (tok != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", tok);
        compute_message(message, line);

        memset(line, 0, LINELEN);
    }

    // Step 4 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL)
    {
        offset = 0;
        sprintf(line, "Cookie: %s", cookies[0]);
        offset += strlen(line);

        for (int i = 1; i < cookies_count; i++)
        {
            sprintf(line + offset, "; %s", cookies[i]);
            offset += strlen(cookies[i]) + 2;
        }

        compute_message(message, line);
    }

    memset(line, 0, LINELEN);

    // Step 5: add final new line
    compute_message(message, "");

    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                           int body_data_fields_count, char **cookies, int cookies_count, char *tok)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    int offset;

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);

    compute_message(message, line);

    memset(line, 0, LINELEN);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    memset(line, 0, LINELEN);

    // Step 3 (optional): add token
    if (tok != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", tok);
        compute_message(message, line);

        memset(line, 0, LINELEN);
    }

    /* Step 4: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    int content_length = 0;

    for (int i = 0; i < body_data_fields_count; i++)
    {
        content_length += strlen(body_data[i]) + 1;
    }

    content_length--;

    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    memset(line, 0, LINELEN);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);

    // Step 5 (optional): add cookies
    if (cookies != NULL)
    {
        offset = 0;
        sprintf(line, "Cookies: %s", cookies[0]);
        offset += strlen(line);

        for (int i = 1; i < cookies_count; i++)
        {
            sprintf(line + offset, "; %s", cookies[i]);
            offset += strlen(cookies[i]) + 2;
        }

        compute_message(message, line);
    }
    // Step 6: add new line at end of header
    compute_message(message, "");

    memset(line, 0, LINELEN);

    // Step 7: add the actual payload data
    if (body_data_fields_count != 0)
    {

        offset = 0;
        sprintf(body_data_buffer, "%s", body_data[0]);
        offset += strlen(body_data[0]);

        for (int i = 1; i < body_data_fields_count; i++)
        {
            sprintf(body_data_buffer + offset, "&%s", body_data[i]);
            offset += strlen(body_data[i]) + 1;
        }

        strcat(message, body_data_buffer);
    }

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *tok)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    int offset;

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    memset(line, 0, LINELEN);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    memset(line, 0, LINELEN);

    // Step 3 (optional): add token
    if (tok != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", tok);
        compute_message(message, line);

        memset(line, 0, LINELEN);
    }

    // Step 4 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL)
    {
        offset = 0;
        sprintf(line, "Cookie: %s", cookies[0]);
        offset += strlen(line);

        for (int i = 1; i < cookies_count; i++)
        {
            sprintf(line + offset, "; %s", cookies[i]);
            offset += strlen(cookies[i]) + 2;
        }

        compute_message(message, line);
    }

    memset(line, 0, LINELEN);

    // Step 5: add final new line
    compute_message(message, "");

    return message;
}
