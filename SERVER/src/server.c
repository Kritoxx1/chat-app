#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <threads.h>
#include <string.h>

#define PORT 8080
#define MAX_CLIENTS 2
#define USERNAME_LEN 32
#define MESSAGE_LEN 1024

typedef int id;

typedef struct {
    char username[USERNAME_LEN];
    char message[MESSAGE_LEN];
} ClientInfo;

typedef struct {
    int client_fd;
    int other_client_fd;
    ClientInfo client_info;
} ClientArgs;

int HandleClients(void *arg) {
    ClientArgs *args = (ClientArgs *)arg;
    int client_fd = args->client_fd;
    int other_client_fd = args->other_client_fd;
    char *username = args->client_info.username;
    char message[MESSAGE_LEN];

    while (1) {
        memset(message, 0, MESSAGE_LEN);
        if (recv(client_fd, message, MESSAGE_LEN, 0) <= 0) {
            printf("Client %s disconnected.\n", username);
            close(client_fd);
            free(args);
            return 0;
        }
        printf("%s: %s\n", username, message);

        if (send(other_client_fd, message, strlen(message), 0) == -1) {
            printf("Failed to send message to %s\n", username);
            close(client_fd);
            free(args);
            return 0;
        }
    }
    return 0;
}

int main(void) {
    int server_fd, client_fd, client_fd2;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    id client_id1 = 0, client_id2 = 0;
    ClientInfo client1_info, client2_info;
    char test[] = "[SERVER] THIS IS A TEST";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("ERR: Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERR: Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is running on port %d\n", PORT);

    /*Accepting the first Client*/
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("ERR: Accept failed");
    } else {
        recv(client_fd, client1_info.username, USERNAME_LEN - 1, 0);
        client1_info.username[USERNAME_LEN - 1] = '\0';
        recv(client_fd, &client_id1, sizeof(client_id1), 0);
        printf("Client1: %s (ID: %d)\n", client1_info.username, client_id1);
	
	send(client_fd, test, strlen(test), 0);
    }
	
    /*Accepting the second Client*/
    client_fd2 = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd2 < 0) {
        perror("ERR: Accept failed");
    } else {
        recv(client_fd2, client2_info.username, USERNAME_LEN - 1, 0);
        client2_info.username[USERNAME_LEN - 1] = '\0';
        recv(client_fd2, &client_id2, sizeof(client_id2), 0);
        printf("Client2: %s (ID: %d)\n", client2_info.username, client_id2);
	
	send(client_fd2, test, strlen(test), 0);
    }

    send(client_fd, test, strlen(test), 0);

    ClientArgs *args1 = malloc(sizeof(ClientArgs));
    args1->client_fd = client_fd;
    args1->other_client_fd = client_fd2;
    args1->client_info = client1_info;

    ClientArgs *args2 = malloc(sizeof(ClientArgs));
    args2->client_fd = client_fd2;
    args2->other_client_fd = client_fd;
    args2->client_info = client2_info;

    thrd_t thread1, thread2;
    thrd_create(&thread1, HandleClients, args1);
    thrd_create(&thread2, HandleClients, args2);

    thrd_join(thread1, NULL);
    thrd_join(thread2, NULL);

    close(server_fd);
    printf("Server stopped.\n");
    return 0;
}

