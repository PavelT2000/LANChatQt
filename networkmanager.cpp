#include "networkmanager.h"

NetworkManager::NetworkManager(ushort port, QObject *parent)
    : QObject(parent), m_port(port)
{
    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(QHostAddress::Any, m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    m_tcpServer = new QTcpServer(this);
    m_tcpServer->listen(QHostAddress::Any, m_port);
    m_myAddr=getMyAddr();

    connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onUdpReadyRead);
    connect(m_tcpServer, &QTcpServer::newConnection, this, &NetworkManager::onNewTcpConnection);

    qDebug()<<"Конструктор networkManager адрес:"<<m_myAddr.toString()<<" порт:"<<m_port;

}

bool NetworkManager::sendDataBroadcast(const QByteArray &data)
{
    qint64 bytesSent = m_udpSocket->writeDatagram(data, QHostAddress::Broadcast, m_port);
    qDebug()<<"Отправленно "<<bytesSent<<" байт данных через broadcast";
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
        if(datagram.senderAddress()==m_myAddr)
        {
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

QHostAddress NetworkManager::getMyAddr()
{
    QList<QHostAddress> addrs=QNetworkInterface::allAddresses();
    for(QHostAddress &addr : addrs)
    {
        if(addr.protocol()==QHostAddress::IPv4Protocol &&
            (addr.isInSubnet(QHostAddress::parseSubnet("192.168.0.0/16"))||
            addr.isInSubnet(QHostAddress::parseSubnet("10.0.0.0/8")) ||
            addr.isInSubnet(QHostAddress::parseSubnet("172.16.0.0/12"))))
        {
            return addr;
        }
    }
    return QHostAddress();
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