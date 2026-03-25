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
    qDebug()<<"Network manager:"<<"Отправленно "<<bytesSent<<" байт данных через broadcast";
    return (bytesSent != -1);
}

bool NetworkManager::sendDataTo(const QByteArray &data, QTcpSocket &target)
{
    qint64 bytesWritten=target.write(data);
    qDebug()<<"Network manager:"<<"Отправлено "<<bytesWritten<<" байт по "<<target.peerAddress();
    return (bytesWritten != -1 && target.flush());

}



void NetworkManager::onUdpReadyRead() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        qDebug()<<"Network manager: "<<"get UDP from "<<datagram.senderAddress().toString();

        if(datagram.senderAddress().isEqual(m_myAddr,QHostAddress::TolerantConversion))
        {
            continue;
        }
        emit dataReceived(datagram.data(), datagram.senderAddress(), Protocol::UDP);
    }
}

void NetworkManager::onNewTcpConnection() {
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::onTcpReadyRead);
    emit peerConnected(*clientSocket);
}

void NetworkManager::onTcpReadyRead() {


    QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
    if (s) {
        QByteArray readData=s->readAll();
        qDebug()<<"Read "<<readData.length()<<" bytes from"<<s->peerAddress();
        qDebug()<<"Network manager:" <<
            "Получено "<<readData.length()<<" байт от "<<s->peerAddress().toString();
        emit dataReceived(readData, s->peerAddress(), Protocol::TCP);
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



QTcpSocket * NetworkManager::setConnection(QHostAddress &addr)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(addr, m_port);
    return socket;

}

void NetworkManager::deleteConnection(QTcpSocket &target)
{
    target.disconnect();
    target.deleteLater();
}

// void NetworkManager::disconnectIp(QHostAddress ip)
// {
//     auto it = tcpConnections.begin();
//     while (it != tcpConnections.end()) {
//         QTcpSocket* socket = *it;

//         if (socket->peerAddress().toIPv4Address() == ip.toIPv4Address()) {


//             socket->disconnectFromHost();

//             it = tcpConnections.erase(it);
//             socket->deleteLater();
//         } else {
//             ++it;
//         }
//     }
// }
