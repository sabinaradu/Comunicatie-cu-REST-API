#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>

#include "parson.h"
#include "helpers.h"
#include "requests.h"

#define PORT 8080
#define HOST "34.118.48.238"

char *postMessage(char *jsonString, char *url, char *auth) {

	char *message = compute_post_request(HOST, url, "application/json", jsonString, NULL, auth);
	int socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	send_to_server(socket, message);
	char *serverResponse = receive_from_server(socket);
	printf("%s\n", serverResponse);

	close(socket);
	return serverResponse;
}

char *getMessage(char *url, char *cookie, char *auth) {

	char *message = compute_get_request(HOST, url, NULL, cookie, auth);
	int socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	send_to_server(socket, message);
	char *serverResponse = receive_from_server(socket);
	printf("%s\n", serverResponse);

	close(socket);
	return serverResponse;
}


int main(int argc, char const *argv[])
{
	char *command = malloc(50);
	char *serverResponse;
	char *p;
	char *token = NULL;
	char *cookie = NULL;

	while(command) {
		memset(command, 0, 50);
		scanf("%s", command);
		if (strcmp(command, "register") == 0) {
			char username[100];
			printf("username=");
			scanf("%s", username);
			char password[100];
			printf("password=");
			scanf("%s", password);

			JSON_Value *jsonValue;
			JSON_Object *jsonObject;
			jsonValue = json_value_init_object();
			jsonObject = json_value_get_object(jsonValue);
			json_object_set_string(jsonObject, "username", username);
			json_object_set_string(jsonObject, "password", password);
			postMessage(json_serialize_to_string(jsonValue), 
				"/api/v1/tema/auth/register", NULL);
		}
		else if (strcmp(command, "login") == 0) {
			if (cookie != NULL) {
				printf("\nYou are already logged in to an account!\n\n");
				continue;
			}
			char username[100];
			printf("username=");
			scanf("%s", username);
			char password[100];
			printf("password=");
			scanf("%s", password);

			JSON_Value *jsonValue;
			JSON_Object *jsonObject;
			jsonValue = json_value_init_object();
			jsonObject = json_value_get_object(jsonValue);
			json_object_set_string(jsonObject, "username", username);
			json_object_set_string(jsonObject, "password", password);
			serverResponse = postMessage(json_serialize_to_string(jsonValue),
				"/api/v1/tema/auth/login", NULL);

			p = strstr(serverResponse, "Set-Cookie");
			if(p) {
				p += 12;
				cookie = strtok(p, " ");
			}	
		}
		else if (strcmp(command, "enter_library") == 0) {
			
			if (cookie) {
				serverResponse = getMessage("/api/v1/tema/library/access", cookie, NULL);

				p = strstr(serverResponse, "{");

				JSON_Value *jsonValue;
				JSON_Object *jsonObject;
				jsonValue = json_parse_string(p);
				jsonObject = json_value_get_object(jsonValue);
				token = (char*)json_object_get_string(jsonObject, "token");
			}
			else {
				printf("\nYou are not logged in!\n\n");
				
			}
		}
		else if (strcmp(command, "get_books") == 0) {
			if (cookie == NULL) {
				printf("\nYou are not logged in!\n\n");
				continue;
			}
			if (token == NULL) {
				printf("\nYou do not have access to the library!\n\n");
				continue;
			}
			serverResponse = getMessage("/api/v1/tema/library/books", NULL, token);
		}
		else if (strcmp(command, "get_book") == 0) {
			
			if (cookie && token) {
				printf("id=");

				char *id = malloc(10);
				char *url = malloc(50);

				scanf("%s", id);
				strcpy(url, "/api/v1/tema/library/books/");
				strcat(url, id);

				free(id);
				serverResponse = getMessage(url, NULL, token);
			}
			if (!cookie) {
				printf("\nYou are not logged in!\n\n");
				continue;
			}
			if (!token) {
				printf("\nYou do not have access to the library!\n\n");
				continue;
			}
		}
		else if (strcmp(command, "add_book") == 0) {
			if (cookie == NULL) {
				printf("\nYou are not logged in!\n\n");
				continue;
			}
			if (token == NULL) {
				printf("\nYou do not have access to the library!\n\n");
				continue;
			}

			char read[10];
			char *title = malloc(100);
			fgets(read, 10, stdin);
			printf("title=");
			fgets(title, 100, stdin);
			title = strtok(title, "\n");

			char *author = malloc(50);
			printf("author=");
			fgets(author, 50, stdin);
			author = strtok(author, "\n");

			char *genre = malloc(50);
			printf("genre=");
			fgets(genre, 50, stdin);
			genre = strtok(genre, "\n");

			char *page_count = malloc(10);
			printf("page_count=");
			fgets(page_count, 10, stdin);
			page_count = strtok(page_count, "\n");

			char *publisher = malloc(50);
			printf("publisher=");
			fgets(publisher, 50, stdin);
			publisher = strtok(publisher, "\n");

			JSON_Value *jsonValue;
			JSON_Object *jsonObject;
			jsonValue = json_value_init_object();
			jsonObject = json_value_get_object(jsonValue);
			json_object_set_string(jsonObject, "title", title);
			json_object_set_string(jsonObject, "author", author);
			json_object_set_string(jsonObject, "genre", genre);
			json_object_set_number(jsonObject, "page_count", (double)atoi(page_count));
			json_object_set_string(jsonObject, "publisher", publisher);

			
			serverResponse = postMessage(json_serialize_to_string(jsonValue),
				"/api/v1/tema/library/books", token);
		}
		else if (strcmp(command, "delete_book") == 0) {
			if (cookie == NULL) {
				printf("\nYou are not logged in!\n\n");
				continue;
			}
			if (token == NULL) {
				printf("\nYou do not have access to the library!\n\n");
				continue;
			}
			printf("id=");
			char *id = malloc(10);
			scanf("%s", id);
			char *url = malloc(50);
			strcpy(url, "/api/v1/tema/library/books/");
			strcat(url, id);

			int socket = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			char *message = compute_delete_request(HOST, url, "application/json", token);
			send_to_server(socket, message);
			char *server_response = receive_from_server(socket);
			printf("%s\n", server_response);

			close(socket);
			serverResponse = server_response;
		}
		else if (strcmp(command, "exit") == 0) {
			break;
		}
		else if (strcmp(command, "logout") == 0) {
			serverResponse = getMessage("/api/v1/tema/auth/logout", cookie, NULL);
			cookie = NULL;
			token = NULL;
		}
		else {
			printf("\nInvalid command\n\n");
		}
	}

	return 0;
}