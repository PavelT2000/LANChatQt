#include "enterance.h"
#include "ui_enterance.h"
#include <QMessageBox>

Enterance::Enterance(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Enterance)
{
    ui->setupUi(this);
}

Enterance::~Enterance()
{
    delete ui;
}

void Enterance::on_btnConnect_clicked()
{
    const QString name = ui->leName->text().trimmed();
    bool ok = false;
    const int portInt = ui->lePort->text().toInt(&ok);

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Invalid input", "Name cannot be empty.");
        return;
    }
    if (!ok || portInt < 1 || portInt > 65535) {
        QMessageBox::warning(this, "Invalid input", "Port must be between 1 and 65535.");
        return;
    }

    emit Entered(name, static_cast<ushort>(portInt));
    this->accept();
}

