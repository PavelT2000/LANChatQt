#include "chatengine.h"

ChatEngine::ChatEngine(ushort port, QString name, QObject *parent)
    :QObject{parent}, m_name(name)
{
    m_netMan=new NetworkManager(port, this);
    m_aliveTimer = new QTimer(this);
    m_aliveTimer->start(5000);

    connect(m_netMan, &NetworkManager::dataReceived, this, &ChatEngine::handlePocket);
    connect(m_netMan, &NetworkManager::peerDisconnected, this, &ChatEngine::disconnectPeer);
    connect(m_aliveTimer, &QTimer::timeout, this, &ChatEngine::timerTick);


    heartBeat();
    emit peersUpdated(m_peers);
    qDebug()<<"Chat Engine: "<<"Конструктор чата";

}

QString ChatEngine::getName()
{
    return m_name;
}

void ChatEngine::setName(const QString & name)
{
    timerTick();
    m_name=name;
    emit peersUpdated(m_peers);
}



void ChatEngine::handlePocket(const QByteArray &data, const QHostAddress &senderIp, Protocol protocol)
{
    Packet msg=Packet::fromBytes(data);
    switch(msg.type)
    {
    case MessageType::ALIVE:
        setAlivePeer(msg.senderName,senderIp);
        break;
    case MessageType::MESSAGE:
        emit messageReceived(msg.senderName,msg.content);
        break;
    default:

        break;

    }
}

void ChatEngine::setAlivePeer(QString newName, QHostAddress ip)
{
    qDebug()<<"ChatEngine: "<<"Получен HeartBeat от "<<ip.toString();
    auto it = m_peers.find(ip.toString());
    if (it != m_peers.end()) {
        Peer& p = it.value(); // Работаем со ссылкой на существующий объект
        p.liveStatus=0;
        if(p.name!=newName)
        {

            qDebug()<<"ChatEngine: "<<ip.toString()<<" сменил имя с "<<p.name<<" на "<<newName;
            p.name=newName;
        }
    }
}

void ChatEngine::sendAliveStatus()
{
    m_netMan->sendDataBroadcast(Packet(MessageType::ALIVE,m_name).toBytes());
}

void ChatEngine::timerTick()
{
    heartBeat();


}

void ChatEngine::disconnectPeer(QHostAddress &addr)
{
    bool ok;
    quint32 ipv4 = addr.toIPv4Address(&ok);
    QString ipStr = ok ? QHostAddress(ipv4).toString() : addr.toString();
    auto it = m_peers.find(ipStr);
    if (it != m_peers.end()) {
        Peer peer=it.value();
        qDebug()<<"ChatEngine:"<<peer.name<<" покинул чат";
        if (m_peers.contains(ipStr)) {
            qDebug()<<"ChatEngine:" << "Удаляем пира из списка:" << peer.name;
            m_peers.remove(ipStr);
            emit peersUpdated(m_peers);
        }
    }
}

void ChatEngine::heartBeat()
{
    qDebug()<<"ChatEngine: "<<"HeartBeat";
    m_netMan->sendDataBroadcast(Packet(MessageType::ALIVE,m_name).toBytes());
}

void ChatEngine::updatePeersState()
{
    auto it = m_peers.begin();
    while (it != m_peers.end()) {
        it.value().liveStatus++;

        if (it.value().liveStatus > 2) {
            qDebug() <<"ChatEngine: "<<it->name<<"("<<it.key()<<") не ответил три раза, удаляем...";
            m_netMan->deleteConnection(it->socket);
            it = m_peers.erase(it);
            emit peersUpdated(m_peers);
        } else {
            ++it;
        }
    }
}

void ChatEngine::sendMessage(QString text)
{
    Packet pkg(MessageType::MESSAGE, m_name, text);
    QByteArray data = pkg.toBytes();

    for (const Peer &peer : m_peers) {
        m_netMan->sendDataTo(data, peer.socket);
        qDebug()<<"ChatEngine: " << "Отправлено сообщения для:" << peer.name << "на IP:" << peer.socket.peerAddress().toString();
    }
}

