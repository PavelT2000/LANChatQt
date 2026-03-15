#include "networkmanager.h"

NetworkManager::NetworkManager(ushort port, QObject *parent)
    : QObject(parent), m_port(port)
{
    // Настройка UDP
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onUdpReadyRead);

    // Настройка TCP Сервера (ждем входящие)
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, m_port);
    connect(tcpServer, &QTcpServer::newConnection, this, &NetworkManager::onNewTcpConnection);
}

bool NetworkManager::sendDataBroadcast(const QByteArray &data)
{
    qint64 bytesSent = udpSocket->writeDatagram(data, QHostAddress::Broadcast, m_port);
    return (bytesSent != -1);
}

bool NetworkManager::sendDataTo(const QByteArray &data, const QHostAddress &targetIp)
{
    for (QTcpSocket* s : tcpConnections) {
        if (s->peerAddress() == targetIp) {
            qint64 bytesWritten = s->write(data);
            s->flush();
            return (bytesWritten != -1 && s->flush());
        }
    }
    return false; // Пир с таким IP не найден в активных соединениях
}



void NetworkManager::onUdpReadyRead() {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QHostAddress senderIp = datagram.senderAddress();

        // 1. Приводим адрес отправителя к чистому IPv4
        bool ok;
        quint32 senderIpv4 = senderIp.toIPv4Address(&ok);

        bool isOwnIp = false;
        const QList<QHostAddress> localhostAddresses = QNetworkInterface::allAddresses();

        for (const QHostAddress &address : localhostAddresses) {
            // 2. Приводим локальный адрес к чистому IPv4 для сравнения
            bool localOk;
            quint32 localIpv4 = address.toIPv4Address(&localOk);

            // Если оба адреса удалось привести к IPv4 и они совпали
            if (ok && localOk && senderIpv4 == localIpv4) {
                isOwnIp = true;
                break;
            }

            // На всякий случай проверяем прямое совпадение (для чистого IPv6)
            if (senderIp == address) {
                isOwnIp = true;
                break;
            }
        }

        if (isOwnIp) {
            continue;
        }

        emit dataReceived(datagram.data(), senderIp, Protocol::UDP);
    }
}

void NetworkManager::onNewTcpConnection() {
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    tcpConnections.append(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::onTcpReadyRead);
}

void NetworkManager::onTcpReadyRead() {
    qDebug() << "Tcp";
    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    if (s) {
        emit dataReceived(s->readAll(), s->peerAddress(), Protocol::TCP);
    }
}

void NetworkManager::establishConnection(const QHostAddress &ip) {
    for (QTcpSocket* s : tcpConnections)
    {
        if (s->peerAddress() == ip) return;
    }
    QTcpSocket *socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onTcpReadyRead);


    connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
        tcpConnections.removeAll(socket);
        socket->deleteLater();
        qDebug() << "Соединение разорвано:" << socket->peerAddress().toString();
    });

    socket->connectToHost(ip, m_port);

    tcpConnections.append(socket);
    qDebug() << "Попытка установить связь с:" << ip.toString();
}