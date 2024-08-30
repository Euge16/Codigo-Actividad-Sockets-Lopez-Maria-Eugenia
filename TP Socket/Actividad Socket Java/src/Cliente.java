import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Cliente {
    

    public static void main(String[] args) {
        try (Socket socket = new Socket("localhost", 5000);
             DataOutputStream output = new DataOutputStream(socket.getOutputStream());
             BufferedReader input = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {

            Scanner scanner = new Scanner(System.in);
            boolean continuar = true;

            while (continuar) {
                // Mostrar el menú
                System.out.println("=======================================MENU PRINCIPAL=======================================");
                System.out.println("1. Generar nombre de usuario");
                System.out.println("2. Generar contraseña");
                System.out.println("3. Cerrar sesión");
                System.out.print("Seleccione una opción: ");
                int opcion = scanner.nextInt();

                // Enviar la opción seleccionada al servidor
                output.writeInt(opcion);

                // Procesar la opción
                if (opcion == 1 || opcion == 2) {
                    System.out.print("Ingrese el tamaño: ");
                    int tam = scanner.nextInt();
                    output.writeInt(tam); // Enviar el tamaño al servidor
                }

                // Leer la respuesta del servidor
                String respuesta = input.readLine();  // Leer hasta el salto de línea
                System.out.println("Servidor: " + respuesta);

                if (opcion == 3) {
                    continuar = false;  // Salir del bucle si se selecciona cerrar sesión
                }

            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}









