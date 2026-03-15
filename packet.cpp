#include "packet.h"
#include <QIODevice>
Packet::Packet(MessageType t, QString name, QString msg)
    : type(t), senderName(name), content(msg) {}

QByteArray Packet::toBytes() const {
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << static_cast<qint8>(type) << senderName << content;
    return buffer;
}

Packet Packet::fromBytes(const QByteArray &data) {
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_6_0);
    qint8 t;
    QString n, c;
    in >> t >> n >> c;
    return Packet(static_cast<MessageType>(t), n, c);
}