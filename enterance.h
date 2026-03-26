#ifndef ENTERANCE_H
#define ENTERANCE_H

#include <QDialog>

namespace Ui {
class Enterance;
}

class Enterance : public QDialog
{
    Q_OBJECT

public:
    explicit Enterance(QWidget *parent = nullptr);
    ~Enterance();

signals:
    void Entered(QString name, ushort port);

private slots:


    void on_btnConnect_clicked();

private:
    Ui::Enterance *ui;
};

#endif // ENTERANCE_H
