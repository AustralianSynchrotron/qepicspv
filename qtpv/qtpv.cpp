#include "qtpv.h"
#include "src/QCaObject.h"

#include <QTime>
#include <QCoreApplication>
#include <QDebug>




bool qtWait(const QList<ObjSig> & osS, int delay) {

  QEventLoop q;
  foreach(ObjSig os, osS)
    QObject::connect(os.sender, os.signal, &q, SLOT(quit()));

  QTimer * tT = 0;
  if (delay>0) {
    tT = new QTimer;
    tT->setSingleShot(true);
    QObject::connect(tT, SIGNAL(timeout()), &q, SLOT(quit()));
    tT->start(delay);
  }

  q.exec();

  bool ret = true;
  if (tT) {
    ret = tT->isActive();
    delete tT;
  }
  return ret;

}

bool qtWait(const QObject * sender, const char * signal, int delay) {
  QList<ObjSig> osS;
  osS << ObjSig(sender,signal);
  return qtWait(osS,delay);
}

bool qtWait(int delay){

  if (delay <= 0)
    return true;

  QTimer * tT = 0;
  tT = new QTimer;
  tT->setSingleShot(true);
  QEventLoop q;

  QObject::connect(tT, SIGNAL(timeout()), &q, SLOT(quit()));

  tT->start(delay);
  q.exec();

  delete tT;

  return false;

}








using namespace qcaobject;

const QVariant QEpicsPv::badData = QVariant();

const bool QEpicsPv::inited = QEpicsPv::init();

unsigned QEpicsPv::debugLevel = 0;

bool QEpicsPv::init() {
  qRegisterMetaType<QCaConnectionInfo>("QCaConnectionInfo&");
  qRegisterMetaType<QCaAlarmInfo>("QCaAlarmInfo&");
  qRegisterMetaType<QCaDateTime>("QCaDateTime&");
  qRegisterMetaType<QVariant>("QVariant&");
  qRegisterMetaType<QVariant>("QVariant");
  return true;
}

void QEpicsPv::setDebugLevel(unsigned level){
  debugLevel = level;
}

QEpicsPv::QEpicsPv(const QString & _pvName, QObject *parent) :
  QObject(parent),
  qCaField(0),
  pvName(_pvName),
  lastData(),
  updated(false),
  theEnum()
{
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: INI" << this << _pvName;
  setPV(pvName);
}

QEpicsPv::QEpicsPv(QObject *parent) :
  QObject(parent),
  qCaField(0),
  pvName(),
  lastData(),
  updated(false),
  theEnum()
{
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: INI" << this;
}


QEpicsPv::~QEpicsPv(){
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: DEL" << this << isConnected() << pv();
  if ( ! pv().isEmpty() )
    setPV();
}


void QEpicsPv::setPV(const QString & _pvName) {
  pvName = _pvName;
  setObjectName(pvName);
  // to guarantee that the preSetPV is not called before the QCoreApplication::exec()
  QTimer::singleShot(0,this,SLOT(preSetPV()));
}


void QEpicsPv::preSetPV() {

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: SPV" << this << pvName ;

  if (qCaField) {
    qCaField->deleteChannel();
    delete qCaField;
    qCaField = 0;
  }
  updateConnection();

  if ( pvName.isEmpty() ) {
    emit pvChanged(pvName);
    return;
  }

  qCaField = new QCaObject(pvName, this);

  // Qt::QueuedConnection here is needed to ensure the QEventLoop in
  // ::getUpdated() and ::getReady() methods are running smoothly.
  //
  // This requirement to use the queued signal and slot connections
  // caused the necessity to register the corresponding types using
  // qRegisterMetaType() functions.
  //
  connect(qCaField, SIGNAL(connectionChanged(QCaConnectionInfo&)),
          SLOT(updateConnection()), Qt::QueuedConnection);
  connect(qCaField, SIGNAL(dataChanged(QVariant,QCaAlarmInfo&,QCaDateTime&)),
          SLOT(updateValue(QVariant)), Qt::QueuedConnection);

  qCaField->subscribe();

  emit pvChanged(pvName);


}





const QString & QEpicsPv::pv() const {
  return pvName;
}


bool QEpicsPv::isConnected() const {
  return qCaField
      && qCaField -> isChannelConnected()
      && get().isValid();
}

const QVariant & QEpicsPv::get() const {
  return lastData;
}

void QEpicsPv::needUpdated() const {
  updated = false;
}

const QVariant & QEpicsPv::getUpdated(int delay) const {

  if ( ! qCaField )
    return badData;
  if ( updated )
    return lastData;

  QList<ObjSig> osS;
  osS
      << ObjSig(this, SIGNAL(valueUpdated(QVariant)))
      << ObjSig(this, SIGNAL(connectionChanged(bool)));
  qtWait(osS, delay);

  QCoreApplication::processEvents(); // needed to process updateValue
  return ( ! isConnected() || ! updated )
      ? badData : lastData;

}



const QVariant & QEpicsPv::getConnected(int delay) const {
  if ( ! qCaField )
    return badData;
  if ( ! isConnected() )
    qtWait(this, SIGNAL(connected()), delay);
  return lastData;
}




QVariant QEpicsPv::get(const QString & _pvName, int delay) {
  if ( _pvName.isEmpty() )
    return badData;
  QEpicsPv tpv(_pvName);
  QCoreApplication::processEvents();
  return tpv.getConnected(delay);
}

const QVariant & QEpicsPv::set(QVariant value, int delay) {

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: SET" << this << isConnected() << pv() << get() << value << getEnum();

  if ( ! isConnected() || ! value.isValid() )
    return badData ;

  if (delay >= 0)
    needUpdated();

  if ( getEnum().size() ) {
    if ( ! getEnum().contains(value.toString()) ) {
      bool ok;
      qlonglong val = value.toLongLong(&ok);
      if (!ok) {
        qDebug() << "ERROR in QEpicsPv! Value" << value << "to set the PV" << pv()
            << "of the enum type could not be found in the list of possible values"
            << getEnum() << "and could not be converted into integer.";
        return badData;
      }
      if ( val >= getEnum().size() || val < 0 ) {
        qDebug() << "ERROR in QEpicsPv! Value" << value << "to set the PV" << pv()
            << "of the enum type, when converted into integer" << val
            << "is not a valid index in the list of possible values"
            << getEnum() << ".";
        return badData;
      }
      value = val;
    }
  } else if ( get().type() != value.type()  && ! value.convert(get().type()) ) {
    qDebug() << "ERROR in QEpicsPv! Could not convert type QVariant from \"" << value.typeName()
             << "\" to \"" << get().typeName() << "\" to set the PV" << pv();
    return badData;
  }

  qCaField->writeData(value);

  return delay >= 0  ?  getUpdated(delay)  :  get();

}



QVariant QEpicsPv::set(const QString & _pvName, const QVariant & value, int delay) {
  if (_pvName.isEmpty())
    return badData;
  QEpicsPv tpv(_pvName);
  QCoreApplication::processEvents();
  return tpv.getConnected().isValid()  ?  tpv.set(value, delay)  :  badData;
}


void QEpicsPv::updateValue(const QVariant & data){

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: UPD" << this << isConnected() << pv() << get() << data << getEnum();

  if ( ! qCaField || ! data.isValid() )
    return;

  updated = true;
  bool firstRead = ! lastData.isValid();
  bool changed = firstRead || (lastData != data);
  lastData = data;

  if (firstRead) {
    theEnum = qCaField->getEnumerations();
    emit connected();
    emit connectionChanged(true);
  }
  if (changed)
    emit valueChanged(lastData);
  emit valueUpdated(lastData);

}


void QEpicsPv::updateConnection() {

  bool con =  qCaField && qCaField->isChannelConnected();
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: CON" << this << pv() << con ;
  if (con)
    return;

  updated=false;
  lastData = badData;
  theEnum.clear();
  emit disconnected();
  emit connectionChanged(false);

}

const QStringList & QEpicsPv::getEnum() const {
  return theEnum;
}
