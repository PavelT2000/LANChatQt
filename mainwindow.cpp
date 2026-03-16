#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateUserList(const QMap<QString, Peer> &peers) {
    ui->listWidget->clear(); // Очищаем старый список
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

void MainWindow::on_textEdit_textChanged()
{
    chat = new ChatEngine(12345,ui->textEdit->toPlainText(),this);
    connect(chat, &ChatEngine::messageReceived, this, &MainWindow::displayMessage);
    connect(chat, &ChatEngine::peersUpdated, this, &MainWindow::updateUserList);
}

