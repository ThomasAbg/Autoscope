#include "tcp_server.h"
#include "tcp_client.h"

int main(int argc, char *argv[]){
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    // Autostart localhost server
    TcpServer tcp_server(true);

    TcpClient tcp_client;

    // Connect to localhost server
    tcp_client.connectToHost("127.0.0.1", 4444);

    QTcpSocket *tcp_client_from_server = NULL;

    while(!tcp_server.waitForNewConnection());

    tcp_client_from_server = tcp_server.get_client();

    tcp_client_from_server->write("Hello world!");

    while(!tcp_client_from_server->waitForBytesWritten());

    while(!tcp_client.waitForReadyRead());

    qDebug() << "from server:" << tcp_client.readAll();
}
