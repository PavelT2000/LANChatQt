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
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendMessage);
    connect(chat, &ChatEngine::messageReceived, this, &MainWindow::onMessageReceived);
    const QList<QHostAddress> localhostAddresses = QNetworkInterface::allAddresses();
    NetworkManager* nm=new NetworkManager(10000,this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateUserList(const QMap<QString, Peer> &peers) {
    ui->listWidget->clear();
    ui->listWidget->addItem(QString("%1 %2 (%3)")
                                .arg("●", chat->getName(),"local"));
    for (const Peer &peer : peers) {
        QString status = (peer.liveStatus == 0) ? "●" : "○";
        ui->listWidget->addItem(QString("%1 %2 (%3)")
                                    .arg(status, peer.name, peer.ip.toString()));
    }
}
void MainWindow::displayMessage(QString name, QString text) {
    QString formattedMsg = QString("<b>%1:</b> %2").arg(name, text);
    ui->textBrowser->append(formattedMsg);
}

void MainWindow::onSendMessage()
{
    QString text = ui->lineEdit->text();
    if (text.isEmpty()) return;

    chat->sendMessage(text);
    ui->lineEdit->clear();
    ui->textBrowser->append(QString("<b>%1:</b> <font color='green'>%2</font>")
                                                 .arg(chat->getName())
                                                 .arg(text));
}

void MainWindow::onMessageReceived(QString name, QString text)
{
    ui->textBrowser->append(QString("<b>%1:</b> <font color='green'>%2</font>")
                                                 .arg(name)
                                                 .arg(text));
}

void MainWindow::on_textEdit_textChanged()
{
    if(chat==nullptr)
    {

        chat = new ChatEngine(12345,ui->textEdit->toPlainText(),this);
        qDebug()<<"чат создан имя:"<<chat->getName();
        connect(chat, &ChatEngine::messageReceived, this, &MainWindow::displayMessage);
        connect(chat, &ChatEngine::peersUpdated, this, &MainWindow::updateUserList);
    }
    else
    {
        chat->setName(ui->textEdit->toPlainText());
        qDebug()<<"имя обновленно имя:"<<chat->getName();
    }


}



void MainWindow::on_pushButton_clicked()
{

}

