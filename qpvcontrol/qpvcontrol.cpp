#include "qpvcontrol.h"
#include "ui_qtpvgui.h"
#include "ui_qpvcontrol.h"
#include <QCheckBox>
#include <QDir>
#include <QFileDialog>


const QString QEpicsPvApp::configsSearchBaseDir = "qepicspvapp";

QEpicsPvApp::QEpicsPvApp(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::QEpicsPvApp),
  pvsFile()
{

  //QEpicsPv::setDebugLevel(1);

  ui->setupUi(this);

#if QT_VERSION >= 0x050000
  ui->table->verticalHeader()->setSectionsMovable(true);
  ui->table->verticalHeader()->setSectionsClickable(true);
  ui->table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
  ui->table->verticalHeader()->setMovable(true);
  ui->table->verticalHeader()->setClickable(true);
  ui->table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  ui->table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

  connect(ui->add, SIGNAL(clicked()), SLOT(addPv()));
  connect(ui->clear, SIGNAL(clicked()), SLOT(clear()));
  connect(ui->table->verticalHeader(), SIGNAL(sectionDoubleClicked(int)),
          SLOT(removeRow(int)));

  QLabel * conf = new QLabel("Configuration:");
  ui->statusBar->addPermanentWidget(conf);

  QPushButton * save = new QPushButton("Save");
  save->setFlat(true);
  connect(save, SIGNAL(clicked()), SLOT(onSave()));
  ui->statusBar->addPermanentWidget(save);

  QPushButton * load = new QPushButton("Load");
  load->setFlat(true);
  connect(load, SIGNAL(clicked()), SLOT(onLoad()));
  ui->statusBar->addPermanentWidget(load);

  QCheckBox * lock = new QCheckBox("Lock GUI");
  connect(lock, SIGNAL(toggled(bool)), save, SLOT(setHidden(bool)));
  connect(lock, SIGNAL(toggled(bool)), load, SLOT(setHidden(bool)));
  connect(lock, SIGNAL(toggled(bool)), conf, SLOT(setHidden(bool)));
  connect(lock, SIGNAL(toggled(bool)), ui->manipulate, SLOT(setHidden(bool)));
  connect(lock, SIGNAL(toggled(bool)), ui->table->verticalHeader(), SLOT(setHidden(bool)));
  lock->setCheckState(Qt::Unchecked);
  ui->statusBar->addPermanentWidget(lock);


  const QString configPath = QString(getenv("HOME")) + "/." + configsSearchBaseDir;
  QDir configDir(configPath);
  if ( ! configDir.exists() )
    configDir.mkpath(configPath);
  pvsFile.setFileName(configPath +  "/listOfPvsInUI.txt");

  if ( pvsFile.open(QIODevice::ReadWrite | QIODevice::Text) &&
       pvsFile.isReadable() )
    while ( ! pvsFile.atEnd() ) {
      QByteArray line = pvsFile.readLine();
      addPv(line.trimmed(), true);
    }
  if ( ! pvsFile.isWritable() )
    pvsFile.close();

}


QEpicsPvApp::~QEpicsPvApp() {
  pvsFile.close();
  clear();
  delete ui;
}


QEpicsPvGUI * QEpicsPvApp::addPv(const QString &presetpv, bool noFileSave) {
  QEpicsPvGUI * pv = new QEpicsPvGUI(presetpv, this);
  addPv(pv, noFileSave);
  return pv;
}

void QEpicsPvApp::addPv(QEpicsPvGUI *pv, bool noFileSave) {

  int idx = ui->table->rowCount();
  ui->table->insertRow(idx);
  ui->table->setCellWidget(idx,0,pv->basicUI()->pv);
  ui->table->setCellWidget(idx,1,pv->basicUI()->set);
  ui->table->setCellWidget(idx,2,pv->basicUI()->get);

  connect(pv, SIGNAL(valueChanged(QVariant)), SLOT(resetHeader()));

  pvs[pv->basicUI()->pv] = pv;

  QTableWidgetItem * itm = new QTableWidgetItem("-");
  itm->setToolTip("Drag to reorganize, double-click to remove.");
  ui->table->setVerticalHeaderItem( idx, itm );

  if ( ! noFileSave )
    updatePvsFile();

}


void QEpicsPvApp::removeRow(int idx) {

  if (idx<0)
    return;

  QMLineEdit * pvedt = (QMLineEdit*) ui->table->cellWidget(idx, 0);
  ui->table->removeRow( idx );
  delete pvs[pvedt];
  pvs.remove(pvedt);

  updatePvsFile();

}


void QEpicsPvApp::removePv(QEpicsPvGUI *_pv) {

  if ( !_pv )
    return;

  const QMLineEdit * pvedt = _pv->basicUI()->pv;
  const int rowCount = ui->table->rowCount();
  int idx=0;
  while ( idx < rowCount )
    if (ui->table->cellWidget(idx,0) == pvedt)
      break;
    else
      idx++;

  if (idx < rowCount )
    removeRow(idx);

}



void QEpicsPvApp::clear(){
  for (int crow = ui->table->rowCount() - 1 ;  crow >= 0 ; --crow )
    removeRow(crow);
}


QList < QEpicsPvGUI * > QEpicsPvApp::pvsList() const {
  QList<QEpicsPvGUI*> list;
  for (int vidx = 0 ;  vidx < ui->table->rowCount() ; ++vidx ) {
    int lidx = ui->table->verticalHeader()->logicalIndex(vidx);
    list << pvs[ (QMLineEdit*) ui->table->cellWidget(lidx, 0) ];
  }
  return list;
}


void QEpicsPvApp::updatePvsFile() {
  if ( ! pvsFile.isOpen() )
    return;
  pvsFile.reset();
  pvsFile.resize(0);
  foreach (QEpicsPvGUI * pv, pvsList() )
    #if QT_VERSION >= 0x050000
    pvsFile.write( ( pv->pv() + "\n" ).toLatin1());
    #else
    pvsFile.write( ( pv->pv() + "\n" ).toAscii());
    #endif
  pvsFile.flush();
}


void QEpicsPvApp::resetHeader() {
  // Here the delay of 100 msec is set to allow the setup button's text be
  // updated before resetting the header.
  QTimer::singleShot(100, ui->table->horizontalHeader(), SLOT(reset()));
}

void QEpicsPvApp::saveConfiguration(const QString & fileName) {
  QFile file(fileName);
  if ( file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) &&
       file.isWritable() )
    foreach (QEpicsPvGUI * pv, pvsList() )
      #if QT_VERSION >= 0x050000
      file.write( ( pv->pv() + "\n" ).toLatin1());
      #else
      file.write( ( pv->pv() + "\n" ).toAscii());
      #endif
  file.close();
}

void QEpicsPvApp::loadConfiguration(const QString & fileName) {
  QFile file(fileName);
  if ( file.open(QIODevice::ReadOnly | QIODevice::Text) &&
      file.isReadable() ) {
    clear();
    while (!file.atEnd()) {
      QByteArray line = file.readLine();
      addPv(line.trimmed());
    }
  }
  file.close();
}


void QEpicsPvApp::onSave() {
  saveConfiguration( QFileDialog::getSaveFileName(0, "Save configuration") );
}



void QEpicsPvApp::onLoad() {
  loadConfiguration(QFileDialog::getOpenFileName(0, "Load configuration"));
}
