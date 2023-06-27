#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define HOST "34.254.242.81"
#define PORT 8080
#define TYPE "application/json"
#define URL_REGISTER "/api/v1/tema/auth/register"
#define URL_LOGIN "/api/v1/tema/auth/login"
#define URL_ACCESS "/api/v1/tema/library/access"
#define URL_BOOKS "/api/v1/tema/library/books"
#define URL_LOGOUT "/api/v1/tema/auth/logout"

#define MAXLEN 50
#define NMAX 2048

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd = -1;

    char buffer[MAXLEN + 1];
    memset(buffer, 0, MAXLEN + 1);

    char **body_data = calloc(1, sizeof(char *));
    char **cookies = malloc(1 * sizeof(char *));
    cookies[0] = calloc(NMAX, sizeof(char));
    char *token = NULL;

    char username[MAXLEN];
    char password[MAXLEN];

    JSON_Value *value = json_value_init_object();
    JSON_Object *object;

    while (1)
    {
        if (fgets(buffer, sizeof(buffer), stdin) && !isspace(buffer[0]))
        {
            // process user command
            if (strncmp(buffer, "exit", strlen("exit")) == 0)
            {
                break;
            }
            else if (strncmp(buffer, "register", 8) == 0)
            {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                // read username and password from stdin
                printf("username = ");
                scanf("%s", username);

                printf("password = ");
                scanf("%s", password);

                // parse them in json fromat
                object = json_value_get_object(value);
                json_object_set_string(object, "username", username);
                json_object_set_string(object, "password", password);

                body_data[0] = json_serialize_to_string(value);

                message = compute_post_request(HOST, URL_REGISTER, TYPE, body_data, 1, NULL, 0, NULL);
                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                if (strstr(response, "error"))
                {
                    printf("The username %s is taken!\n", username);
                }
                else
                {
                    printf("Account has been created, please log in!\n");
                }
            }
            else if (strncmp(buffer, "login", strlen("login")) == 0)
            {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                // read username and password from stdin
                printf("username = ");
                scanf("%s", username);

                printf("password = ");
                scanf("%s", password);

                // parse them in json fromat
                object = json_value_get_object(value);
                json_object_set_string(object, "username", username);
                json_object_set_string(object, "password", password);

                body_data[0] = json_serialize_to_string(value);

                message = compute_post_request(HOST, URL_LOGIN, TYPE, body_data, 1, NULL, 0, NULL);
                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                // get session cookie
                char *sesh_cookie = strstr(response, "Set-Cookie: ");

                if (sesh_cookie != NULL)
                {
                    sesh_cookie = strtok(sesh_cookie, ";");
                    sesh_cookie += strlen("Set-Cookie: ");

                    strcpy(cookies[0], sesh_cookie);

                    printf("Welcome, %s!\n", username);
                }
                else
                {
                    printf("Could not login, try again!\n");
                }
            }
            else if (strncmp(buffer, "logout", strlen("logout")) == 0)
            {
                // verify the user is logged in first
                if (strncmp(cookies[0], "connect.sid", 11) == 0)
                {
                    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                    message = compute_get_request(HOST, URL_LOGOUT, NULL, cookies, 1, NULL);
                    send_to_server(sockfd, message);

                    response = receive_from_server(sockfd);

                    // reset session cookie and token to block access
                    memset(cookies[0], 0, NMAX);
                    token = NULL;

                    printf("Goodbye, %s!\n", username);
                }
                else
                {
                    printf("Could not log out, log in first!\n");
                }
            }
            else if (strncmp(buffer, "enter_library", strlen("enter_library")) == 0)
            {
                // verify the user is logged in first
                if (strncmp(cookies[0], "connect.sid", strlen("connect.sid")) == 0)
                {
                    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                    message = compute_get_request(HOST, URL_ACCESS, NULL, cookies, 1, NULL);
                    send_to_server(sockfd, message);

                    response = receive_from_server(sockfd);

                    // get access token
                    char *access_tok = strstr(response, "{\"token\":\"");

                    if (access_tok != NULL)
                    {
                        access_tok += strlen("{\"token\":\"");
                        access_tok = strtok(access_tok, "\"");

                        token = calloc(NMAX, sizeof(char));
                        strcpy(token, access_tok);

                        printf("Welcome inside the library, %s!\n", username);
                    }
                    else
                    {
                        printf("Access denied!\n");
                    }
                }
                else
                {
                    printf("Could not enter library, log in first!\n");
                }
            }
            else if (strncmp(buffer, "get_books", strlen("get_books")) == 0)
            {
                // verify the user is logged in first
                if (strncmp(cookies[0], "connect.sid", strlen("connect.sid")) == 0)
                {
                    // verify that user has access
                    if (token != NULL)
                    {
                        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                        message = compute_get_request(HOST, URL_BOOKS, NULL, cookies, 1, token);
                        send_to_server(sockfd, message);

                        response = receive_from_server(sockfd);

                        printf("Books: %s\n", strstr(response, "["));
                    }
                    else
                    {
                        printf("Enter library first!\n");
                    }
                }
                else
                {
                    printf("Could not enter library, log in first!\n");
                }
            }
            else if (strncmp(buffer, "get_book", strlen("get_book")) == 0)
            {
                // verify the user is logged in first
                if (strncmp(cookies[0], "connect.sid", strlen("connect.sid")) == 0)
                {
                    // verify that user has access
                    if (token != NULL)
                    {
                        int id;
                        printf("id  = ");
                        scanf("%d", &id);

                        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                        // assemble path to desired book
                        char book_path[MAXLEN], book_id[MAXLEN];
                        sprintf(book_id, "/%d", id);
                        strcpy(book_path, URL_BOOKS);
                        strcat(book_path, book_id);

                        message = compute_get_request(HOST, book_path, NULL, cookies, 1, token);
                        send_to_server(sockfd, message);

                        response = receive_from_server(sockfd);

                        // verify if the book exists
                        if (strstr(response, "error"))
                        {
                            printf("No book with that id found, try again!\n");
                        }
                        else
                        {
                            printf("Book: %s\n", strstr(response, "{"));
                        }
                    }
                    else
                    {
                        printf("Enter library first!\n");
                    }
                }
                else
                {
                    printf("Could not enter library, log in first!\n");
                }
            }
            else if (strncmp(buffer, "add_book", strlen("add_book")) == 0)
            {
                // verify the user is logged in first
                if (strncmp(cookies[0], "connect.sid", strlen("connect.sid")) == 0)
                {
                    // verify that user has access
                    if (token != NULL)
                    {
                        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                        char title[NMAX], author[NMAX], genre[NMAX], publisher[NMAX], page_count_str[NMAX];
                        int page_count;

                        // read book details from stdin
                        printf("title  = ");
                        fgets(title, NMAX, stdin);
                        strtok(title, "\n");

                        printf("author  = ");
                        fgets(author, NMAX, stdin);
                        strtok(author, "\n");

                        printf("genre  = ");
                        fgets(genre, NMAX, stdin);
                        strtok(genre, "\n");

                        printf("publisher  = ");
                        fgets(publisher, NMAX, stdin);
                        strtok(publisher, "\n");

                        printf("page_count  = ");
                        scanf("%d", &page_count);

                        sprintf(page_count_str, "%d", page_count);
                        printf("%d\n", page_count);

                        // parse them in json fromat
                        object = json_value_get_object(value);
                        json_object_set_string(object, "title", title);
                        json_object_set_string(object, "author", author);
                        json_object_set_string(object, "genre", genre);
                        json_object_set_string(object, "publisher", publisher);
                        json_object_set_string(object, "page_count", page_count_str);

                        body_data[0] = json_serialize_to_string(value);

                        message = compute_post_request(HOST, URL_BOOKS, TYPE, body_data, 1, NULL, 0, token);
                        send_to_server(sockfd, message);

                        response = receive_from_server(sockfd);

                        // verify that the book has been added successfully
                        if (strstr(response, "error"))
                        {
                            printf("Could not add book, try again!\n");
                        }
                        else
                        {
                            printf("Book \"%s\" has been added!\n", title);
                        }
                    }
                    else
                    {
                        printf("Enter library first!\n");
                    }
                }
                else
                {
                    printf("Could not enter library, log in first!\n");
                }
            }
            else if (strncmp(buffer, "delete_book", strlen("delete_book")) == 0)
            {
                // verify the user is logged in first
                if (strncmp(cookies[0], "connect.sid", strlen("connect.sid")) == 0)
                {
                    // verify that user has access
                    if (token != NULL)
                    {
                        int id;
                        printf("id  = ");
                        scanf("%d", &id);

                        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                        // assemble path to desired book
                        char book_path[MAXLEN], book_id[MAXLEN];
                        sprintf(book_id, "/%d", id);
                        strcpy(book_path, URL_BOOKS);
                        strcat(book_path, book_id);

                        // compute delete request
                        message = compute_delete_request(HOST, book_path, NULL, cookies, 1, token);
                        send_to_server(sockfd, message);

                        response = receive_from_server(sockfd);

                        // verify if the book exists
                        if (strstr(response, "error"))
                        {
                            printf("No book with that id found, try again!\n");
                        }
                        else
                        {
                            printf("Book successfully deleted!\n");
                        }
                    }
                    else
                    {
                        printf("Enter library first!\n");
                    }
                }
                else
                {
                    printf("Could not enter library, log in first!\n");
                }
            }
            else
            {
                printf("Try another command!\n");
            }
        }
    }

    // free all resources
    free(body_data);
    free(cookies[0]);
    free(cookies);

    if (token != NULL)
    {
        free(token);
    }

    if (sockfd == -1)
    {
        close_connection(sockfd);
    }
    return 0;
}
