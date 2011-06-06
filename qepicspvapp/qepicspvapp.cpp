#include "qepicspvapp.h"


QEpicsPvapp::QEpicsPvapp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QEpicsPvapp)
{
  QEpicsPv::setDebugLevel(1);
    ui->setupUi(this);
    new QEpicsPvGUI(ui->centralWidget);
}

QEpicsPvapp::~QEpicsPvapp()
{
    delete ui;
}
