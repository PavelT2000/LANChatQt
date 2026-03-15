#ifndef CHATENGINE_H
#define CHATENGINE_H

#include <QObject>
#include <QMap>
#include <QHostAddress>
#include "networkmanager.h"
#include "packet.h"

class ChatEngine : public QObject
{
    Q_OBJECT
public:
    explicit ChatEngine(ushort port, QString name, QObject *parent = nullptr);

private:
    NetworkManager * nm;
    QMap<QString,Peer> peers;
    QString name;
    void handlePocket(const QByteArray &data, const QHostAddress &senderIp, Protocol protocol);
    void setAlive(QString name, QHostAddress ip);
    void sendAliveStatus();
    void sendMessage(QString text);

signals:
};

#endif // CHATENGINE_H
