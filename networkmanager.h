#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <QUdpSocket>
#include <QObject>
#include <QDebug>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>

struct Peer {
    QHostAddress ip;
    QString name;
    int liveStatus;
};

enum Protocol { TCP, UDP };

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(ushort port, QObject *parent = nullptr);

    // ОТДАТЬ(данные, протокол, пир)
    // Если для UDP пир не указан (Null), шлем Broadcast
    bool sendDataBroadcast(const QByteArray &data);
    bool sendDataTo(const QByteArray &data, const QHostAddress &targetIp);
    void establishConnection(const QHostAddress &ip);
    void disconnectIp(QHostAddress ip);

signals:
    // ПОЛУЧИТЬ(данные, пир_отправитель)
    void dataReceived(const QByteArray &data, const QHostAddress &senderIp, Protocol protocol);
    void peerDisconnected(QHostAddress &addr);

private slots:
    void onUdpReadyRead();
    void onNewTcpConnection();
    void onTcpReadyRead();


private:
    QUdpSocket *udpSocket;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> tcpConnections;
    ushort m_port;

};


#endif // NETWORKMANAGER_H
