#include "packet.h"
#include <QIODevice>
#include <QDataStream> // Обязательно добавь этот инклуд для работы со стримами

Packet::Packet(MessageType t, QString name, QString msg)
    : type(t), senderName(name), content(msg) {}

QByteArray Packet::toBytes() const {
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);

    // Используем версию 5.15 для совместимости с твоим GitHub Action
    out.setVersion(QDataStream::Qt_5_15);

    out << static_cast<qint8>(type) << senderName << content;
    return buffer;
}

Packet Packet::fromBytes(const QByteArray &data) {
    QDataStream in(data);

    // Версии при чтении и записи ДОЛЖНЫ совпадать
    in.setVersion(QDataStream::Qt_5_15);

    qint8 t;
    QString n, c;
    in >> t >> n >> c;
    return Packet(static_cast<MessageType>(t), n, c);
}
