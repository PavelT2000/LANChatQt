#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkInterface>
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug()<<"Запуск приложения";
    chat=nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initChat(QString name, ushort port)
{

    chat = new ChatEngine(port,name,this);
    connect(chat, &ChatEngine::messageReceived, this, &MainWindow::displayMessage);
    connect(chat, &ChatEngine::peersUpdated, this, &MainWindow::updateUserList);
    qDebug()<<"чат создан имя:"<<chat->getName();
    chat->updatePeers();


}

void MainWindow::updateUserList(const QMap<QString, Peer*> &peers) {
    ui->lstPeers->clear();
    ui->lstPeers->addItem(QString("%1 %2 (%3)")
                                .arg("●", chat->getName(),"local"));
    for (Peer *peer : peers) {
        QString status = (peer->liveStatus == 0) ? "●" : "○";
        ui->lstPeers->addItem(QString("%1 %2 (%3)")
                                    .arg(status, peer->name, peer->socket->peerAddress().toString()));
    }
}
void MainWindow::displayMessage(QString name, QString text) {
    QString formattedMsg = QString("<b>%1:</b> %2").arg(name, text);
    ui->tbChatField->append(formattedMsg);
}

void MainWindow::onSendMessage()
{
    QString text = ui->leInputField->text();
    if (text.isEmpty()) return;

    chat->sendMessage(text);
    ui->leInputField->clear();
    ui->tbChatField->append(QString("<b>%1:</b> <font color='green'>%2</font>")
                                                 .arg(chat->getName())
                                                 .arg(text));
}

void MainWindow::onMessageReceived(QString name, QString text)
{
    ui->tbChatField->append(QString("<b>%1:</b> <font color='green'>%2</font>")
                                                 .arg(name)
                                                 .arg(text));
}




void MainWindow::on_btnEnterMessage_clicked()
{
    onSendMessage();
}


void MainWindow::on_leInputField_returnPressed()
{
    onSendMessage();
}

