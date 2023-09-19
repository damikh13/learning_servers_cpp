// Server side implementation of UDP client-server model
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

const int PORT_NUMBER = 2023;
const int BUF_SZ = 1024;

int main()
{
    // BUFFER FOR RECEIVING DATA
    char buffer[BUF_SZ];
    const char* msg_to_send = "Hello from server";

    // CREATING SOCKET FILE DESCRIPTOR
    int socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    //                                  ^^^^^^^  ^^^^^^^^^^  ^
    //                                  IPv4     UDP         auto choose protocol for IPv4 and UDP

    // check if socket creation was successful
    bool socket_creation_was_unsuccessful = (socket_file_descriptor < 0);
    if (socket_creation_was_unsuccessful)
    {
        // print error message to stderr
        perror("socket creation failed");
        // exit with failure code
        exit(EXIT_FAILURE);
        //   ^^^^^^^^^^^^
        //   EXIT_FAILURE is a macro defined in <cstdlib> that expands to a platform-dependent integral expression
    }

    // FILLING SERVER INFORMATION
    sockaddr_in server_address_info;
    memset(&server_address_info, 0, sizeof(server_address_info)); // fill with zeros
    server_address_info.sin_family = AF_INET; // IPv4
    // server_address_info.sin_addr.s_addr = inet_addr("217.197.0.187");
    server_address_info.sin_addr.s_addr = INADDR_ANY;
    server_address_info.sin_port = htons(PORT_NUMBER);
    //                             ^^^^^^^^^^^^^^^^^
    //                             converts the port number to network byte order (big endian)

    // BINDING SOCKET TO SERVER ADDRESS
    int res = bind(
        socket_file_descriptor,
        (const struct sockaddr*)&server_address_info,
        sizeof(server_address_info));
    // check if binding was successful
    bool binding_was_unsuccessful = (res < 0);
    if (binding_was_unsuccessful)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // RECEIVE MESSAGE FROM CLIENT
    sockaddr_in client_address_information;
    memset(&client_address_information, 0, sizeof(client_address_information)); // fill with zeros
    socklen_t client_address_information_size = sizeof(client_address_information);
    int number_of_bytes_received = recvfrom(
        socket_file_descriptor, // socket to read from
        (char*)buffer, // buffer to read into
        BUF_SZ, // size of buffer
        0, // flags (0 for no flags)
        (struct sockaddr*)&client_address_information, // client address information
        &client_address_information_size // size of client address information
    );
    buffer[number_of_bytes_received] = '\0';
    std::cout << "Message from client: " << buffer << std::endl;

    // SEND RESPONSE MESSAGE TO CLIENT 
    sendto(
        socket_file_descriptor, // socket to send from
        msg_to_send,
        strlen(msg_to_send),
        0, // flags (0 for no flags)
        (const struct sockaddr*)&client_address_information, // client address information
        client_address_information_size // size of client address information
    );
    std::cout << "Hello message sent." << std::endl;

    // CLOSE SOCKET
    close(socket_file_descriptor);

    return 0;
}