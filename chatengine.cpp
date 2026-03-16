#include "chatengine.h"

ChatEngine::ChatEngine(ushort port, QString name, QObject *parent)
    :name(name), QObject{parent}
{
    nm=new NetworkManager(port, this);

    connect(nm, &NetworkManager::dataReceived, this, &ChatEngine::handlePocket);
    connect(nm, &NetworkManager::peerDisconnected, this, &ChatEngine::peerDisconected);
    aliveTimer = new QTimer(this);
    connect(aliveTimer, &QTimer::timeout, this, &ChatEngine::timerTick);
    aliveTimer->start(5000);
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
        emit messageReceived(msg.senderName,msg.content);
        break;
    case MessageType::DEACTIVE:break;

    }
}

void ChatEngine::setAlive(QString name, QHostAddress ip)
{
    QString ipStr = ip.toString();
    if (peers.contains(ipStr)) {
        if(peers[ipStr].name != name)
        {
            peers[ipStr].name = name;
            emit peersUpdated(peers);
        }

        peers[ipStr].liveStatus =0;
    } else {

        Peer newPeer;
        newPeer.ip = ip;
        newPeer.name = name;
        newPeer.liveStatus = 0;
        nm->establishConnection(newPeer.ip);
        peers.insert(ipStr, newPeer);
        emit peersUpdated(peers);
        sendAliveStatus();
        qDebug() << "Обнаружен новый узел:" << name << "[" << ipStr << "]";
    }
}

void ChatEngine::sendAliveStatus()
{
    nm->sendDataBroadcast(Packet(MessageType::ALIVE,name).toBytes());
}

void ChatEngine::timerTick()
{
    qDebug()<<"HeartBeat";
    nm->sendDataBroadcast(Packet(MessageType::ALIVE,name).toBytes());
    auto it = peers.begin();
    while (it != peers.end()) {
        // Увеличиваем счетчик отсутствия
        it.value().liveStatus++;

        // Если пир не подавал признаков жизни более 3-х проверок
        if (it.value().liveStatus > 2) {
            qDebug() << "Удаляем замолчавшего:" << it.value().name;
            nm->disconnectIp(it.value().ip);
            it = peers.erase(it);
            emit peersUpdated(peers);
        } else {
            ++it;
        }
    }
}

void ChatEngine::peerDisconected(QHostAddress &addr)
{
    bool ok;
    quint32 ipv4 = addr.toIPv4Address(&ok);
    QString ipStr = ok ? QHostAddress(ipv4).toString() : addr.toString();
    Peer peer=peers[ipStr];
    qDebug()<<peer.name<<" покинул чат";

    // 2. Удаляем по ключу (строке)
    if (peers.contains(ipStr)) {
        qDebug() << "Удаляем пира из списка:" << peers[ipStr].name;
        peers.remove(ipStr);
        emit peersUpdated(peers);
    }
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

