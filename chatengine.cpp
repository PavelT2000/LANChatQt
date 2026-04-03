#include "chatengine.h"

ChatEngine::ChatEngine(ushort port, QString name, QObject *parent)
    :QObject{parent}, m_name(name)
{
    m_netMan=new NetworkManager(port, this);
    m_aliveTimer = new QTimer(this);
    m_aliveTimer->start(5000);

    connect(m_netMan, &NetworkManager::dataReceived, this, &ChatEngine::handlePocket);
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
    m_name = name;
    timerTick();
    emit peersUpdated(m_peers);
}



void ChatEngine::handlePocket(const QByteArray &data, const QHostAddress &senderIp)
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
    if (ip.isLoopback()) return;
    QString ipStr=ip.toString();
    qDebug()<<"ChatEngine: "<<"Получен HeartBeat от "<<ipStr;
    auto it = m_peers.find(ipStr);
    if (it != m_peers.end()) {
        Peer& p = *it.value();
        p.liveStatus=0;
        if(p.name!=newName)
        {

            qDebug()<<"ChatEngine: "<<ipStr<<" сменил имя с "<<p.name<<" на "<<newName;
            p.name=newName;
            emit peersUpdated(m_peers);
        }
    }
    else{
        QTcpSocket *socket = m_netMan->setConnection(ip);
        if (!socket) {
            qWarning() << "ChatEngine: не удалось создать TCP-соединение с" << ipStr;
            return;
        }

        Peer* newPeer = new Peer{socket, newName, 0};
        m_peers.insert(ipStr, newPeer);

        // Удаляем peer по IP, чтобы не зависеть от времени жизни указателя Peer*.
        connect(socket, &QTcpSocket::disconnected, this, [this, ipStr]() {
            this->disconnectPeerByIp(ipStr);
        });
        connect(socket, &QTcpSocket::errorOccurred, this, [this, ipStr](QAbstractSocket::SocketError) {
            this->disconnectPeerByIp(ipStr);
        });

        emit peersUpdated(m_peers);
        heartBeat();
    }
}

void ChatEngine::sendAliveStatus()
{
    m_netMan->sendDataBroadcast(Packet(MessageType::ALIVE,m_name).toBytes());
}

void ChatEngine::timerTick()
{
    heartBeat();
    updatePeersState();
}

void ChatEngine::disconnectPeer(Peer * peer)
{
    if (!peer || !peer->socket)
        return;

    disconnectPeerByIp(peer->socket->peerAddress().toString());
}

void ChatEngine::heartBeat()
{
    qDebug()<<"ChatEngine: "<<"HeartBeat";
    m_netMan->sendDataBroadcast(Packet(MessageType::ALIVE,m_name).toBytes());
}

void ChatEngine::updatePeers()
{
    emit peersUpdated(m_peers);
}

void ChatEngine::updatePeersState()
{
    QStringList toRemove;

    // 1. Сначала только помечаем, кого надо удалить
    for (auto it = m_peers.begin(); it != m_peers.end(); ++it) {
        it.value()->liveStatus++;
        if (it.value()->liveStatus > 3) { // допустим, 3 пропуска
            toRemove << it.key();
        }
    }

    // 2. Вызываем удаление вне основного цикла итерации
    for (const QString& ip : toRemove) {
        qDebug() << "Таймер: удаляем за неактивность" << ip;
        disconnectPeerByIp(ip);
    }
}

void ChatEngine::disconnectPeerByIp(const QString &ipStr)
{
    auto it = m_peers.find(ipStr);
    if (it == m_peers.end())
        return;

    Peer *peer = it.value();
    if (peer) {
        if (peer->socket) {
            m_netMan->deleteConnection(*peer->socket);
        }
        qDebug() << "ChatEngine:" << peer->name << " покинул чат";
        delete peer;
    }

    m_peers.erase(it);
    emit peersUpdated(m_peers);
}

void ChatEngine::sendMessage(QString text)
{
    Packet pkg(MessageType::MESSAGE, m_name, text);
    QByteArray data = pkg.toBytes();

    for (Peer *peer : m_peers) {
        m_netMan->sendDataTo(data, *peer->socket);
        qDebug()<<"ChatEngine: " << "Отправлено сообщения для:" << peer->name << "на IP:" << peer->socket->peerAddress().toString();
    }
}

