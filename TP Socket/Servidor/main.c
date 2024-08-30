#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")


#define BUFFER_SIZE 1024

char* generadorDeNombreDeUsuario(int tam);
char* generadorDeContrasenia(int tam);

int main() {
    srand(time(NULL));

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

    SOCKET servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidorSocket == INVALID_SOCKET) {
        printf("Error al crear el socket\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in servidorAddr;
    servidorAddr.sin_family = AF_INET;
    servidorAddr.sin_port = htons(5000);
    servidorAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(servidorSocket, (struct sockaddr*)&servidorAddr, sizeof(servidorAddr)) == SOCKET_ERROR) {
        printf("Error al vincular el socket al puerto\n");
        closesocket(servidorSocket);
        WSACleanup();
        return 1;
    }

    if (listen(servidorSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Error al escuchar conexiones entrantes\n");
        closesocket(servidorSocket);
        WSACleanup();
        return 1;
    }

    printf("Servidor en espera de conexiones entrantes...\n");

    while (1) {
        SOCKET clienteSocket = accept(servidorSocket, NULL, NULL);
        if (clienteSocket == INVALID_SOCKET) {
            printf("Error al aceptar la conexión\n");
            continue;
        }

        printf("Cliente conectado\n");

        int continuar = 1;
        while (continuar) {

            int opcion;
            int bytesRecibidos = recv(clienteSocket, (char*)&opcion, sizeof(opcion), MSG_WAITALL);
            if (bytesRecibidos != sizeof(opcion)) {
                printf("Error al recibir la opción del cliente\n");
                break;
            }

            // Convertir a endianness de host si es necesario
            opcion = ntohl(opcion);
            printf("Opción recibida: %d\n", opcion);

            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            switch (opcion) {
                case 1: {
                    int tam;
                    int bytesRecibidosTam = recv(clienteSocket, (char*)&tam, sizeof(tam), MSG_WAITALL);
                    if (bytesRecibidosTam != sizeof(tam)) {
                        printf("Error al recibir el tamano\n");
                        strcpy(buffer, "Error al recibir el tamano.");
                        send(clienteSocket, buffer, strlen(buffer), 0);
                        send(clienteSocket, "\n", 1, 0);
                        break;
                    }
                    tam = ntohl(tam);  // Convertir tamaño si es necesario
                    printf("Tamano recibido: %d\n", tam);

                    if (tam < 5 || tam > 15) {
                        strcpy(buffer, "Error: La longitud del usuario debe estar entre 5 y 15 caracteres.\n");
                    } else {
                        char* nombreDeUsuario = generadorDeNombreDeUsuario(tam);
                        if (nombreDeUsuario == NULL) {
                            strcpy(buffer, "Error al generar el nombre de usuario.");
                        } else {
                            snprintf(buffer, BUFFER_SIZE, "Nombre de usuario: %s\n", nombreDeUsuario);
                            free(nombreDeUsuario);
                        }
                    }
                    break;
                }
                case 2: {
                    int tam;
                    int bytesRecibidosTam = recv(clienteSocket, (char*)&tam, sizeof(tam), MSG_WAITALL);
                    if (bytesRecibidosTam != sizeof(tam)) {
                        printf("Error al recibir el tamano\n");
                        strcpy(buffer, "Error al recibir el tamano.");
                        send(clienteSocket, buffer, strlen(buffer), 0);
                        send(clienteSocket, "\n", 1, 0);
                        break;
                    }
                    tam = ntohl(tam);  // Convertir tamaño si es necesario
                    printf("Tamano recibido: %d\n", tam);

                    if (tam < 8 || tam > 50) {
                        strcpy(buffer, "Error: La longitud de la contrasena debe estar entre 8 y 50 caracteres.\n");
                    } else {
                        char* nuevaContrasenia = generadorDeContrasenia(tam);
                        if (nuevaContrasenia == NULL) {
                            strcpy(buffer, "Error al generar la contrasena.");
                        } else {
                            snprintf(buffer, BUFFER_SIZE, "Contrasena: %s\n", nuevaContrasenia);
                            free(nuevaContrasenia);
                        }
                    }
                    break;
                }
                case 3: {
                    strcpy(buffer, "Sesión cerrada\n");
                    continuar = 0;
                    break;
                }
                default:
                    strcpy(buffer, "Opción no válida\n");
                    break;
            }

            printf("Enviando mensaje: %s\n", buffer);
            send(clienteSocket, buffer, strlen(buffer), 0);

            if (opcion == 3) {
                break;
            }
        }

        printf("Cliente desconectado\n");
        closesocket(clienteSocket);
    }

    closesocket(servidorSocket);
    WSACleanup();
    return 0;
}

char* generadorDeNombreDeUsuario(int tam) {
    if (tam <= 0) return NULL;

    char vocales[] = "aeiouAEIOU";
    char consonantes[] = "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ";
    char* usuario = (char*)malloc(tam + 1);
    if (usuario == NULL) return NULL;

    int comienzaVocal = rand() % 2;

    for (int i = 0; i < tam; i++) {
        if ((i % 2 == 0 && comienzaVocal) || (i % 2 != 0 && !comienzaVocal)) {
            usuario[i] = vocales[rand() % (sizeof(vocales) - 1)];
        } else {
            usuario[i] = consonantes[rand() % (sizeof(consonantes) - 1)];
        }
    }
    usuario[tam] = '\0';
    return usuario;
}

char* generadorDeContrasenia(int tam) {
    if (tam <= 0) return NULL;

    char alfanumerica[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char* contrasenia = (char*)malloc(tam + 1);
    if (contrasenia == NULL) return NULL;

    for (int i = 0; i < tam; i++) {
        contrasenia[i] = alfanumerica[rand() % (sizeof(alfanumerica) - 1)];
    }
    contrasenia[tam] = '\0';
    return contrasenia;
}






