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
    QTcpSocket& socket;
    QString name;
    int liveStatus;
};

enum Protocol { TCP, UDP };

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(ushort port, QObject *parent = nullptr);

    bool sendDataBroadcast(const QByteArray &data);
    bool sendDataTo(const QByteArray &data, QTcpSocket &target);
    QTcpSocket& setConnection(QHostAddress &addr);
    void deleteConnection(QTcpSocket &target);
    // void disconnectIp(QHostAddress ip);


signals:
    void dataReceived(const QByteArray &data, const QHostAddress &senderIp, Protocol protocol);
    void peerConnected(QTcpSocket& socket);
    void peerDisconnected(QHostAddress &addr);

private slots:
    void onUdpReadyRead();
    void onNewTcpConnection();
    void onTcpReadyRead();


private:
    QUdpSocket *m_udpSocket;
    QTcpServer *m_tcpServer;
    QHostAddress m_myAddr;
    ushort m_port;
    QHostAddress getMyAddr();

};


#endif
