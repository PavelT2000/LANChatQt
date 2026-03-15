#ifndef CHATENGINE_H
#define CHATENGINE_H

#include <QObject>
#include <QMap>
#include <QHostAddress>
#include <QTimer>
#include "networkmanager.h"
#include "packet.h"

class ChatEngine : public QObject
{
    Q_OBJECT
public:
    explicit ChatEngine(ushort port, QString name, QObject *parent = nullptr);
    void sendMessage(QString text);

private:
    NetworkManager * nm;
    QMap<QString,Peer> peers;
    QString name;
    QTimer *aliveTimer;
    void handlePocket(const QByteArray &data, const QHostAddress &senderIp, Protocol protocol);
    void setAlive(QString name, QHostAddress ip);
    void sendAliveStatus();
    void timerTick();
    void peerDisconected(QHostAddress &addr);


signals:
};

#endif // CHATENGINE_H
