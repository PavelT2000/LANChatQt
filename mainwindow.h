#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatengine.h"

#include <QMap>

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


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_textEdit_textChanged();

private:
    void updateUserList(const QMap<QString, Peer> &peers);
    void displayMessage(QString name, QString text);
    Ui::MainWindow *ui;
    ChatEngine *chat;
};
#endif // MAINWINDOW_H
