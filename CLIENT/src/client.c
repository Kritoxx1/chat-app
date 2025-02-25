#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "<server-IP>" // I forgot to change
#define PORT 8080
#define MESSAGE_LEN 1024

typedef int id;

typedef struct {
    int socket_fd;
} ThreadArgs;

int send_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int socket_fd = args->socket_fd;
    char message[MESSAGE_LEN];

    while (1) {
        printf("Message: ");
        fgets(message, MESSAGE_LEN, stdin);

        if (send(socket_fd, message, strlen(message), 0) < 0) {
            perror("ERR: send() message failed.");
            close(socket_fd);
            exit(0);
        }
    }
    return 0;
}

int recv_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int socket_fd = args->socket_fd;
    char message[MESSAGE_LEN];

    while (1) {
        memset(message, 0, sizeof(message));
        int recv_len = recv(socket_fd, message, MESSAGE_LEN, 0);
        if (recv_len < 0) {
            perror("ERR: recv() message failed.");
            close(socket_fd);
            exit(0);
        }
        printf("\n[MESSAGE]: %s\n", message);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./client <USERNAME> <ID>\n");
        exit(EXIT_FAILURE);
    }

    char username[256];
    strncpy(username, argv[1], sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';

    char message[256];
    char auswahl;
    int client_fd;
    struct sockaddr_in server_addr;

    /*CLIENT ID*/
    id client_id = atoi(argv[2]);

    /*Test text*/
    char test[24];

    /*Creates a Socket*/
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) { 
        perror("ERR: Es gab probleme beim erstellen vom Socket");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, IP, &server_addr.sin_addr.s_addr) <= 0) {
        perror("ERR: Ungueltige Adresse");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    /*Connects with the server*/
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERR: Es gab probleme beim verbinden");
        close(client_fd);
        exit(EXIT_FAILURE);
    } else {
        printf("Connected Succesfully to the server.\n");
    }

    /* Sends the username to the server */
    if (send(client_fd, username, strlen(argv[1]), 0) < 0) {
        perror("ERR: Es gab probleme beim senden");
        close(client_fd);
        exit(EXIT_FAILURE);
    } else {
        printf("Username %s sent to the server\n", username);
    }

    int id = client_id;
    if (send(client_fd, &id, sizeof(id), 0) < 0) {
         perror("ERR: send() id");
    } else {
        printf("The id %d sent to the server\n", id);
    }
   
    int recv_len = recv(client_fd, test, sizeof(test) - 1, 0);
    if (recv_len <= 0) {
        if (recv_len == 0) {
            printf("Server closed the connection.\n");
        } else {
            perror("ERR: recv() test failed");
        }
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    test[recv_len] = '\0'; // Null-Terminierung sicherstellen
    printf("SUCCES RECV: %s\n", test);

    ThreadArgs args = { client_fd };
    thrd_t sendThrd, recvThrd;

    thrd_create(&sendThrd, &send_thread, &args); 
    thrd_create(&recvThrd, &recv_thread, &args);

    thrd_join(sendThrd, NULL);
    thrd_join(recvThrd, NULL);

    close(client_fd);
    return 0;
}
