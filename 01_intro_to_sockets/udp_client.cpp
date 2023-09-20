// Client side implementation of UDP client-server model
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

const int PORT_NUMBER = 2023;
const int BUF_SZ = 1024;
const char* LOCAL_IP_OF_SERVER = "";

int main()
{
    char buffer[BUF_SZ];
    const char* msg_to_send = "Hello from client";

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
    struct sockaddr_in server_address_info;
    memset(&server_address_info, 0, sizeof(server_address_info)); // fill with zeros
    server_address_info.sin_family = AF_INET; // IPv4
    server_address_info.sin_port = htons(PORT_NUMBER);
    //                             ^^^^^^^^^^^^^^^^^
    //                             converts the port number to network byte order (big endian)
    server_address_info.sin_addr.s_addr = inet_addr(LOCAL_IP_OF_SERVER);
    // server_address_info.sin_addr.s_addr = INADDR_ANY;

    // SEND MESSAGE TO SERVER
    socklen_t size_of_server_address_info = sizeof(server_address_info);
    sendto(
        socket_file_descriptor,
        msg_to_send,
        strlen(msg_to_send),
        0,
        (const struct sockaddr*)&server_address_info,
        size_of_server_address_info
    );
    std::cout << "Message sent to server" << std::endl;

    // RECEIVE MESSAGE FROM SERVER
    // set timeout for recvfrom
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;
    setsockopt(socket_file_descriptor, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
    // receive message
    int number_of_bytes_received = recvfrom(
        socket_file_descriptor,
        (char*)buffer,
        BUF_SZ,
        0,
        (struct sockaddr*)&server_address_info,
        &size_of_server_address_info
    );
    // check if recvfrom was successful (i.e. if it received any bytes)
    bool recvfrom_was_unsuccessful = (number_of_bytes_received < 0);
    if (recvfrom_was_unsuccessful)
    {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }
    // print received message
    buffer[number_of_bytes_received] = '\0'; // add null terminator
    std::cout << "Message from server: " << buffer << std::endl;

    // CLOSE SOCKET
    close(socket_file_descriptor);

    return 0;
}