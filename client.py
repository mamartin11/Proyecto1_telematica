import socket
import webbrowser

# Dirección IP y puerto del servidor
server_ip = '10.161.62.74'  # Cambiar por la IP del servidor
server_port = 4455       # Cambiar por el puerto del servidor

# Crear un socket TCP/IP
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Conectar el socket al servidor
server_address = (server_ip, server_port)
print(f"Conectando a {server_ip} puerto {server_port}")
sock.connect(server_address)

def search(url):
       print(url)
       webbrowser.open('https://'+url)

def get_ip():
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
    return ip_address
       
try:
    # Enviar solicitud al servidor
    message = input("Ingrese dominio o IP: ")
    sock.sendall(message.encode())

    # Recibir respuesta del servidor
    response = sock.recv(1024).decode()
    ip=get_ip()
    print(f"Respuesta del servidor: {response}")

    # Actualizar archivo con la solicitud y la respuesta
    with open("cliente_log.txt", "a") as file:
        file.write(f"IP_Cliente: {ip}, Solicitud: {message}, Respuesta: {response}\n")

    # Redirigir a la dirección otorgada por el servidor
    #if message.startswith("www."):
    print(f"Redirigiendo a {response}")
        # Código para redirigir a la dirección
    search(response)

    


finally:
    print("Cerrando conexión")
    sock.close()