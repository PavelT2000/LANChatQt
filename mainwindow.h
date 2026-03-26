#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "chatengine.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void initChat(QString name, ushort port );

private slots:
    void onMessageReceived(QString name, QString text);

    void on_btnEnterMessage_clicked();

    void on_leInputField_returnPressed();

private:
    void updateUserList(const QMap<QString, Peer*> &peers);
    void displayMessage(QString name, QString text);
    void onSendMessage();

    Ui::MainWindow *ui;
    ChatEngine *chat;
};
#endif // MAINWINDOW_H
