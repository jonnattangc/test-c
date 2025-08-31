#include <iostream>
#include <vector>
#include <libssh/libssh.h>
#include <libssh/callbacks.h>

// Función para manejar los errores de libssh
void handle_ssh_error(ssh_session session, const std::string& message) {
    std::cerr << message << ": " << ssh_get_error(session) << std::endl;
    if (session) {
        ssh_disconnect(session);
        ssh_free(session);
    }
    exit(1);
}

int main() {
    ssh_session my_ssh_session;
    int rc;
    
    // --- 1. CONFIGURACIÓN DE LA SESIÓN SSH ---
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL) {
        std::cerr << "Error al crear la sesión SSH." << std::endl;
        return 1;
    }

    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "10.10.0.64");
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, "java");
    
    // Configura la opción de no verificar la clave del host para simplificar el ejemplo
    // En un programa real, esto es una MALA PRÁCTICA de seguridad.
    ssh_options_set(my_ssh_session, SSH_OPTIONS_STRICTHOSTKEYCHECK, "no");

    // --- 2. CONEXIÓN AL SERVIDOR SSH ---
    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK) {
        handle_ssh_error(my_ssh_session, "Error de conexión");
    }

    // --- 3. AUTENTICACIÓN ---
    rc = ssh_userauth_password(my_ssh_session, NULL, "jav@8");
    if (rc != SSH_AUTH_SUCCESS) {
        handle_ssh_error(my_ssh_session, "Error de autenticación");
    }
    std::cout << "Conexión SSH y autenticación exitosas." << std::endl;
    
    // --- 4. CONFIGURACIÓN Y MANEJO DEL REENVÍO DE PUERTO ---
    std::cout << "Creando túnel de reenvío de puerto." << std::endl;
    std::cout << "Redirigiendo localhost:5555 -> 10.10.0.64:22" << std::endl;
    
    // Iniciar el reenvío de puerto local a remoto
    // Esto hace que el servidor SSH escuche en el puerto 5555 de tu máquina local
    rc = ssh_forward_listen(my_ssh_session, "localhost", 5555, NULL);
    if (rc != SSH_OK) {
        handle_ssh_error(my_ssh_session, "Error al iniciar el reenvío de puerto");
    }

    std::cout << "Túnel creado. Puedes acceder al servidor web remoto a través de http://localhost:5555" << std::endl;
    std::cout << "Presiona Ctrl+C para salir." << std::endl;

    // Bucle principal para manejar las conexiones del túnel
    while (true) {
        // Usa ssh_forward_accept para aceptar una nueva conexión entrante
        ssh_channel channel = ssh_forward_accept(my_ssh_session, 2000); // Espera 2 segundos
        if (channel == NULL) {
            continue; // No hay nuevas conexiones, continúa esperando
        }

        std::cout << "Nueva conexión entrante aceptada." << std::endl;
        
        // Ahora necesitamos reenviar esta conexión a la dirección final (servidor web remoto)
        // La conexión ya está establecida, solo necesitamos manejar el tráfico de ida y vuelta.
        
        // Bucle interno para leer y escribir datos a través del canal
        char buffer[256];
        int nbytes;
        
        while (ssh_channel_is_open(channel) && !ssh_channel_is_eof(channel)) {
            // Lee los datos del canal (tráfico que viene del servidor remoto)
            nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
            if (nbytes > 0) {
                // Escribe los datos en la salida estándar
                std::cout.write(buffer, nbytes);
            }
            // Para un programa de producción, deberías usar poll() para manejar 
            // tanto las lecturas como las escrituras de manera eficiente.
        }
        
        std::cout << "Conexión cerrada." << std::endl;
        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    }
    
    // Nunca se alcanza este punto en este ejemplo debido al bucle infinito
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);

    return 0;
}
