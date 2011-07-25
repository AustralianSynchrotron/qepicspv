#include "qtpv.h"
#include "src/QCaObject.h"

#include <QTime>
#include <QCoreApplication>
#include <QDebug>

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
  theEnum(),
  iAmReady(false)
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
  theEnum(),
  iAmReady(false)
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

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: SPV" << this << _pvName ;

  pvName = _pvName;
  if (qCaField) {
    ( (QCaObject *) qCaField )->deleteChannel();
    delete (QCaObject *) qCaField;
    qCaField = 0;
  }
  updateConnection();

  iAmReady = false;
  lastData = QVariant();

  if ( pvName.isEmpty() ) {
    emit pvChanged(pvName);
    return;
  }

  QCaObject * _qCaField = new QCaObject(pvName, this);
  qCaField = _qCaField;

  // Qt::QueuedConnection here is needed to ensure the QEventLoop in
  // ::getUpdated() and ::getReady() methods are running smoothly.
  //
  // This requirement to use the queued signal and slot connections
  // caused the necessity to register the corresponding types using
  // qRegisterMetaType() functions.
  //
  connect(_qCaField, SIGNAL(connectionChanged(QCaConnectionInfo&)),
          SLOT(updateConnection()), Qt::QueuedConnection);
  connect(_qCaField, SIGNAL(dataChanged(QVariant,QCaAlarmInfo&,QCaDateTime&)),
          SLOT(updateValue(QVariant)), Qt::QueuedConnection);

  _qCaField->subscribe();

  emit pvChanged(pvName);


}

const QString & QEpicsPv::pv() const {
  return pvName;
}


bool QEpicsPv::isConnected() const {
  return qCaField && ((QCaObject *) qCaField) -> isChannelConnected();
}

bool QEpicsPv::isReady() const {
  return qCaField && iAmReady;
}

const QVariant & QEpicsPv::get() const {
  return lastData;
}

void QEpicsPv::needUpdated() const {
  updated = false;
}

const QVariant & QEpicsPv::getUpdated(int delay) const {

  if ( ! isConnected() )
    return badData;
  if ( updated )
    return lastData;



  if (delay < 0) delay = 0;

  QEventLoop q;
  QTimer tT;
  tT.setSingleShot(true);

  connect(&tT, SIGNAL(timeout()), &q, SLOT(quit()));
  connect(this, SIGNAL(valueUpdated(QVariant)), &q, SLOT(quit()));
  connect(this, SIGNAL(connectionChanged(bool)), &q, SLOT(quit()));

  if (delay) tT.start(delay);
  q.exec();
  if(tT.isActive()) tT.stop();

  return updated ? lastData : badData ;

}



const QVariant & QEpicsPv::getReady(int delay) const {

  if ( ! qCaField )
    return badData;

  if ( ! isConnected() ) {

    QEventLoop q;
    QTimer tT;
    tT.setSingleShot(true);

    connect(&tT, SIGNAL(timeout()), &q, SLOT(quit()));
    connect(this, SIGNAL(connectionChanged(bool)), &q, SLOT(quit()));

    if (delay) tT.start(delay);
    q.exec();
    if (tT.isActive()) tT.stop();

    if ( ! isConnected() )
      return badData;

  }

  return lastData.isValid()  ?  get()  :  getUpdated(delay);

}




QVariant QEpicsPv::get(const QString & _pvName, int delay) {
  if ( _pvName.isEmpty() )
    return badData;
  QEpicsPv * tpv = new QEpicsPv(_pvName);
  QVariant ret = tpv->getReady(delay);
  delete tpv;
  return  ret;
}

const QVariant & QEpicsPv::set(QVariant value, int delay) {

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: SET" << this << isConnected() << pv() << get() << value << getEnum();

  if ( ! isReady() || ! value.isValid() )
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

  ((QCaObject *) qCaField) -> writeData(value);

  return delay >= 0  ?  getUpdated(delay)  :  get();

}



QVariant QEpicsPv::set(const QString & _pvName, const QVariant & value, int delay) {
  if (_pvName.isEmpty())
    return badData;
  QEpicsPv * tpv = new QEpicsPv(_pvName);
  QVariant ret = tpv->getReady().isValid()  ?  tpv->set(value, delay)  :  badData;
  delete tpv;
  return  ret;
}


void QEpicsPv::updateValue(const QVariant & data){

  if ( ! qCaField )
    return;

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: UPD" << this << isConnected() << pv() << get() << data << getEnum();

  updated = true;
  iAmReady = data.isValid();
  bool firstRead = ! lastData.isValid();
  bool changed = firstRead || (lastData != data);
  lastData = data;

  if (firstRead) {
    theEnum = ((QCaObject *) qCaField) -> getEnumerations();
    emit valueInited(lastData);
  }
  if (changed)
    emit valueChanged(lastData);
  emit valueUpdated(lastData);

}


void QEpicsPv::updateConnection() {
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: CON" << this << pv() << isConnected();
  if (isConnected()) {
    emit connected();
  } else {
    updated=false;
    lastData.clear();
    theEnum.clear();
    emit disconnected();
  }
  emit connectionChanged(isConnected());
}

const QStringList & QEpicsPv::getEnum() const {
  return theEnum;
}
