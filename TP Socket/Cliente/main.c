#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024

void mostrarMenu();

int main() {
    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

    SOCKET clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        printf("Error al crear el socket del cliente\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(5000);
    direccionServidor.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clienteSocket, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        printf("Error al querer conectarse al servidor\n");
        closesocket(clienteSocket);
        WSACleanup();
        return 1;
    }

    int opcion;
    char buffer[BUFFER_SIZE];

    while (1) {
        mostrarMenu();
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        int opcion_red = htonl(opcion);  // Convertir opción a network byte order

        // Enviar la opción seleccionada al servidor
        send(clienteSocket, (char*)&opcion_red, sizeof(opcion_red), 0);

        // Si la opción es 1 o 2, pedir el tamaño
        if (opcion == 1 || opcion == 2) {  // Comparar antes de convertir a network byte order
            int tam;
            printf("Ingrese el tamano: ");
            scanf("%d", &tam);

            int tam_red = htonl(tam);  // Convertir tamaño a network byte order

            // Enviar el tamaño al servidor
            send(clienteSocket, (char*)&tam_red, sizeof(tam_red), 0);
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(clienteSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == SOCKET_ERROR || bytes_received == 0) {
            printf("Error o servidor desconectado\n");
            break;
        }
        buffer[bytes_received] = '\0';  // Hay que asegurarse de que el buffer esté terminado en '\0'
        printf("Servidor: %s\n", buffer);

        if (opcion == 3) {  // Cerrar sesión
            printf("Cerrando sesion...\n");
            break;
        }
    }
    // Cerrar la conexion con el servidor
    closesocket(clienteSocket);
    // Finalizar Winsock
    WSACleanup();
    return 0;
}

void mostrarMenu() {
    printf("=======================================MENU PRINCIPAL=======================================\n");
    printf("1. Generar nombre de usuario\n");
    printf("2. Generar contrasenia\n");
    printf("3. Cerrar sesion\n");
}





