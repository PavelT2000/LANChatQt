#include "enterance.h"
#include "ui_enterance.h"

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
    emit Entered(ui->leName->text(), ui->lePort->text().toInt());
    this->accept();
}

