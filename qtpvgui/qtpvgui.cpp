#include "ui_qtpvgui.h"
#include "qtpvgui.h"
#include <QHBoxLayout>







QEpicsPvGUI::QEpicsPvGUI(QWidget *parent) :
  QEpicsPv(parent),
  theWidget( new QWidget ),
  ui(new Ui::QEpicsPvGUI)
{
  ui->setupUi(theWidget);
  initialize();
}

QEpicsPvGUI::QEpicsPvGUI(const QString & _pvName, QWidget *parent) :
  QEpicsPv(_pvName, parent),
  theWidget( new QWidget ),
  ui(new Ui::QEpicsPvGUI)
{
  ui->setupUi(theWidget);
  initialize();
}

QEpicsPvGUI::~QEpicsPvGUI() {
  delete ui;
  delete theWidget;
}


void QEpicsPvGUI::initialize() {

  connect(ui->pv, SIGNAL(editingFinished(QString)), SLOT(setPV(QString)));

  connect(this, SIGNAL(pvChanged(QString)), ui->pv, SLOT(setText(QString)));
  connect(this, SIGNAL(connectionChanged(bool)), SLOT(onConnectionChange(bool)));
  connect(this, SIGNAL(valueUpdated(QVariant)), SLOT(onValueChange(QVariant)));

}


void QEpicsPvGUI::onSet() {

  QWidget * curWidg = ui->set->currentWidget();

  if ( curWidg == ui->lineW )
    set(ui->lineBox->text());
  else if ( curWidg == ui->doubleW )
    set(ui->doubleBox->value());
  else if ( curWidg == ui->intW )
    set(ui->intBox->value());
  else if ( curWidg == ui->enumW )
    set(ui->enumBox->currentIndex());

}


void QEpicsPvGUI::onConnectionChange(bool con) {

  ui->set->setEnabled(con);
  const QVariant & val = get();

  if ( ! con ) {

    disconnect(ui->lineBox, 0, this, 0);
    disconnect(ui->doubleBox, 0, this, 0);
    disconnect(ui->intBox, 0, this, 0);
    disconnect(ui->enumBox, 0, this, 0);
    ui->get->setText("Disconnected");

  } if ( ! val.isValid() ) {

    // do nothing

  } else if (getEnum().size()) {

    ui->enumBox->clear();
    int idx=0;
    foreach ( QString str, getEnum() )
      ui->enumBox->addItem( "\"" + str + "\" ("+ QString::number(idx++) + ")" );
    ui->enumBox->setCurrentIndex(val.toInt());
    ui->set->setCurrentWidget(ui->enumW);
    connect(ui->enumBox, SIGNAL(indexEdited(int)), SLOT(onSet()));

  } else if ( val.type() == QVariant::Double ) {

    ui->doubleBox->setValue(val.toDouble());
    ui->set->setCurrentWidget(ui->doubleW);
    connect(ui->doubleBox, SIGNAL(valueEdited(double)), SLOT(onSet()));

  } else if ( val.type() == QVariant::LongLong ) {

    ui->intBox->setValue(val.toLongLong());
    ui->set->setCurrentWidget(ui->intW);
    connect(ui->intBox, SIGNAL(valueEdited(int)), SLOT(onSet()));

  } else {

    ui->lineBox->setText(val.toString());
    ui->set->setCurrentWidget(ui->lineW);
    connect(ui->lineBox, SIGNAL(editingFinished(QString)), SLOT(onSet()));

  }

}


void QEpicsPvGUI::onValueChange(const QVariant & val) {
  if ( ! val.isValid() )
    ui->get->setText("Bad data");
  else if (getEnum().size())
    ui->get->setText( "\"" + getEnum()[val.toInt()] + "\" ("+ val.toString() + ")");
  else
    ui->get->setText(val.toString());
}
