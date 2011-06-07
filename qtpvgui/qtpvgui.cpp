#include "qtpvgui.h"
#include <QHBoxLayout>







QEpicsPvGUI::QEpicsPvGUI(QWidget *parent) :
  QEpicsPv(parent),
  theWidget( new QWidget(parent) ),
  ui(new Ui::QEpicsPvGUI)
{
  ui->setupUi(theWidget);
  initialize();
}

QEpicsPvGUI::QEpicsPvGUI(const QString & _pvName, QWidget *parent) :
  QEpicsPv(_pvName, parent),
  theWidget( new QWidget(parent) ),
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
  connect(this, SIGNAL(valueInited(QVariant)), SLOT(onValueInited(QVariant)));
  connect(this, SIGNAL(valueUpdated(QVariant)), SLOT(onValueChange(QVariant)));

  connect(ui->lineBox, SIGNAL(editingFinished(QString)), SLOT(onSet()));
  connect(ui->doubleBox, SIGNAL(valueEdited(double)), SLOT(onSet()));
  connect(ui->intBox, SIGNAL(valueEdited(int)), SLOT(onSet()));
  connect(ui->enumBox, SIGNAL(currentIndexChanged(int)), SLOT(onSet()));

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
  ui->get->setText( con ? "Connected" : "Disconnected" );
  ui->set->setEnabled(con);
}


void QEpicsPvGUI::onValueChange(const QVariant & val) {
  if ( ! val.isValid() )
    ui->get->setText("Bad data");
  else if (getEnum().size())
    ui->get->setText( "\"" + getEnum()[val.toInt()] + "\" ("+ val.toString() + ")");
  else
    ui->get->setText(val.toString());
}


void QEpicsPvGUI::onValueInited(const QVariant & val) {

  if ( ! val.isValid() ) {

  } else if (getEnum().size()) {

    ui->enumBox->clear();
    int idx=0;
    foreach ( QString str, getEnum() )
      ui->enumBox->addItem( "\"" + str + "\" ("+ QString::number(idx++) + ")" );
    ui->enumBox->setCurrentIndex(val.toInt());
    ui->set->setCurrentWidget(ui->enumW);

  } else if ( val.type() == QVariant::Double ) {

    ui->doubleBox->setValue(val.toDouble());
    ui->set->setCurrentWidget(ui->doubleW);

  } else if ( val.type() == QVariant::LongLong ) {

    ui->intBox->setValue(val.toLongLong());
    ui->set->setCurrentWidget(ui->intW);

  } else {

    ui->lineBox->setText(val.toString());
    ui->set->setCurrentWidget(ui->lineW);

  }

  onValueChange(val);

}
