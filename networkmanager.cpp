#include "networkmanager.h"

NetworkManager::NetworkManager(ushort port, QObject *parent)
    : QObject(parent), m_port(port)
{
    // Настройка UDP
    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(QHostAddress::Any, m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onUdpReadyRead);

    // Настройка TCP Сервера (ждем входящие)
    m_tcpServer = new QTcpServer(this);
    m_tcpServer->listen(QHostAddress::Any, m_port);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &NetworkManager::onNewTcpConnection);
    qDebug()<<"Конструктор нетворк";
}

bool NetworkManager::sendDataBroadcast(const QByteArray &data)
{
    qint64 bytesSent = m_udpSocket->writeDatagram(data, QHostAddress::Broadcast, m_port);
    return (bytesSent != -1);
}

bool NetworkManager::sendDataTo(const QByteArray &data, const QHostAddress &targetIp)
{
    for (QTcpSocket*  s : tcpConnections) {
        if (s->peerAddress() == targetIp) {
            qint64 bytesWritten = s->write(data);
            s->flush();
            return (bytesWritten != -1 && s->flush());
        }
    }
    return false;
}



void NetworkManager::onUdpReadyRead() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        QHostAddress senderIp = datagram.senderAddress();
        bool ok;
        quint32 senderIpv4 = senderIp.toIPv4Address(&ok);
        bool isOwnIp = false;
        const QList<QHostAddress> localhostAddresses = QNetworkInterface::allAddresses();

        for (const QHostAddress &address : localhostAddresses) {
            bool localOk;
            quint32 localIpv4 = address.toIPv4Address(&localOk);

            if (ok && localOk && senderIpv4 == localIpv4) {
                isOwnIp = true;
                break;
            }
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
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
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
        QHostAddress addr = socket->peerAddress();
        tcpConnections.removeAll(socket); // Удаляем из списка
        emit peerDisconnected(addr);      // Уведомляем ChatEngine
        socket->deleteLater();            // Безопасно удаляем объект
    });

    socket->connectToHost(ip, m_port);

    tcpConnections.append(socket);
    qDebug() << "Попытка установить связь с:" << ip.toString();
}

void NetworkManager::disconnectIp(QHostAddress ip)
{
    // Используем итератор, так как будем удалять элементы во время обхода
    auto it = tcpConnections.begin();
    while (it != tcpConnections.end()) {
        QTcpSocket* socket = *it;

        // Сравниваем IP (учитывая возможные IPv6-префиксы)
        if (socket->peerAddress().toIPv4Address() == ip.toIPv4Address()) {


            socket->disconnectFromHost();

            it = tcpConnections.erase(it);
            socket->deleteLater();
        } else {
            ++it;
        }
    }
}