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
    QString getName();
    void setName (const QString &name);
    void sendMessage(QString text);
    void heartBeat();

private:
    NetworkManager * m_netMan;
    QMap<QString,Peer*> m_peers;
    QString m_name;
    QTimer *m_aliveTimer;

private:
    void handlePocket(const QByteArray &data, const QHostAddress &senderIp);
    void setAlivePeer(QString name, QHostAddress ip);
    void sendAliveStatus();
    void timerTick();
    void disconnectPeer(Peer * addr);

    void updatePeersState();



signals:
    void messageReceived(QString name, QString text);
    void peersUpdated(const QMap<QString, Peer*> &peers);
};

#endif // CHATENGINE_H
