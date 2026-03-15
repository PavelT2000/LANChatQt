#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include <QByteArray>
#include <QDataStream>

enum class MessageType : qint8 {
    ALIVE = 0,
    DEACTIVE = 1,
    MESSAGE = 2
};

struct Packet {
    MessageType type;
    QString senderName;
    QString content;

    // Конструктор
    Packet(MessageType t = MessageType::ALIVE, QString name = "", QString msg = "");

    // Методы (только объявления)
    QByteArray toBytes() const;
    static Packet fromBytes(const QByteArray &data);
};

#endif // PACKET_H
