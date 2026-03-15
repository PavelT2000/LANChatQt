#include "chatengine.h"

ChatEngine::ChatEngine(ushort port, QString name, QObject *parent)
    :name(name), QObject{parent}
{
    nm=new NetworkManager(port, this);
    nm->sendDataBroadcast(Packet(MessageType::ALIVE,name).toBytes());
    connect(nm, &NetworkManager::dataReceived, this, &ChatEngine::handlePocket);


}



void ChatEngine::handlePocket(const QByteArray &data, const QHostAddress &senderIp, Protocol protocol)
{
    Packet msg=Packet::fromBytes(data);
    switch(msg.type)
    {
    case MessageType::ALIVE:
        setAlive(msg.senderName,senderIp);
        break;
    case MessageType::MESSAGE:
        qDebug()<<msg.content;
        break;
    case MessageType::DEACTIVE:break;

    }
}

void ChatEngine::setAlive(QString name, QHostAddress ip)
{
    QString ipStr = ip.toString();
    if (peers.contains(ipStr)) {
        peers[ipStr].name = name;
        peers[ipStr].liveStatus =0;
    } else {

        Peer newPeer;
        newPeer.ip = ip;
        newPeer.name = name;
        newPeer.liveStatus = 0;
        nm->establishConnection(newPeer.ip);
        peers.insert(ipStr, newPeer);

        qDebug() << "Обнаружен новый узел:" << name << "[" << ipStr << "]";
    }
}

void ChatEngine::sendAliveStatus()
{

}

void ChatEngine::sendMessage(QString text)
{
    Packet pkg(MessageType::MESSAGE, name, text);
    QByteArray data = pkg.toBytes();

    for (const Peer &peer : peers) {
        nm->sendDataTo(data, peer.ip);
        qDebug() << "Отправка сообщения для:" << peer.name << "на IP:" << peer.ip.toString();
    }
}

