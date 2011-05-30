#include "qepicspv.h"
#include "src/QCaObject.h"

#include <QTime>
#include <QCoreApplication>
#include <QDebug>

using namespace qcaobject;

const QVariant QEpicsPV::badData = QVariant();

const bool QEpicsPV::inited = QEpicsPV::init();

unsigned QEpicsPV::debugLevel = 0;

bool QEpicsPV::init() {
  qRegisterMetaType<QCaConnectionInfo>("QCaConnectionInfo&");
  qRegisterMetaType<QCaAlarmInfo>("QCaAlarmInfo&");
  qRegisterMetaType<QCaDateTime>("QCaDateTime&");
  qRegisterMetaType<QVariant>("QVariant&");
  qRegisterMetaType<QVariant>("QVariant");
  return true;
}

void QEpicsPV::setDebugLevel(unsigned level){
  debugLevel = level;
}

QEpicsPV::QEpicsPV(const QString & _pvName, QObject *parent) :
  QObject(parent),
  qCaField(0),
  pvName(_pvName),
  lastData(),
  updated(false),
  theEnum(),
  iAmReady(false)
{
  setPV(pvName);
}

QEpicsPV::QEpicsPV(QObject *parent) :
  QObject(parent),
  qCaField(0),
  pvName(),
  lastData(),
  updated(false),
  theEnum(),
  iAmReady(false)
{
}


QEpicsPV::~QEpicsPV(){
  setPV();
}


void QEpicsPV::setPV(const QString & _pvName) {
  pvName = _pvName;
  if (qCaField) {
    delete (QCaObject *) qCaField;
    qCaField = 0;
  }
  updateConnection();

  iAmReady = false;
  lastData = QVariant();

  if ( pvName.isEmpty() )
    return;

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

}

const QString & QEpicsPV::pv() const {
  return pvName;
}


bool QEpicsPV::isConnected() const {
  return qCaField && ((QCaObject *) qCaField) -> isChannelConnected();
}

bool QEpicsPV::isReady() const {
  return iAmReady;
}

const QVariant & QEpicsPV::get() const {
  return lastData;
}

void QEpicsPV::needUpdated() const {
  updated = false;
}

const QVariant & QEpicsPV::getUpdated(int delay) const {

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



const QVariant & QEpicsPV::getReady(int delay) const {

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




QVariant QEpicsPV::get(const QString & _pvName, int delay) {
  if ( _pvName.isEmpty() )
    return badData;
  QEpicsPV * tpv = new QEpicsPV(_pvName);
  QVariant ret = tpv->getReady(delay);
  delete tpv;
  return  ret;
}

const QVariant & QEpicsPV::set(QVariant value, int delay) {

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPV DEBUG: SET" << this << isConnected() << pv() << get() << value << getEnum();

  if ( ! isReady() || ! value.isValid() )
    return badData ;

  if (delay >= 0)
    needUpdated();

  if ( getEnum().size() ) {
    if ( ! getEnum().contains(value.toString()) ) {
      bool ok;
      qlonglong val = value.toLongLong(&ok);
      if (!ok) {
        qDebug() << "ERROR in QEpicsPV! Value" << value << "to set the PV" << pv()
            << "of the enum type could not be found in the list of possible values"
            << getEnum() << "and could not be converted into integer.";
        return badData;
      }
      if ( val >= getEnum().size() || val < 0 ) {
        qDebug() << "ERROR in QEpicsPV! Value" << value << "to set the PV" << pv()
            << "of the enum type, when converted into integer" << val
            << "is not a valid index in the list of possible values"
            << getEnum() << ".";
        return badData;
      }
      value = val;
    }
  } else if ( get().type() != value.type()  && ! value.convert(get().type()) ) {
    qDebug() << "ERROR in QEpicsPV! Could not convert type QVariant from \"" << value.typeName()
             << "\" to \"" << get().typeName() << "\" to set the PV" << pv();
    return badData;
  }

  ((QCaObject *) qCaField) -> writeData(value);

  return delay >= 0  ?  getUpdated(delay)  :  get();

}



QVariant QEpicsPV::set(QString & _pvName, const QVariant & value, int delay) {
  if (_pvName.isEmpty())
    return badData;
  QEpicsPV * tpv = new QEpicsPV(_pvName);
  QVariant ret = tpv->getReady().isValid()  ?  tpv->set(value, delay)  :  badData;
  delete tpv;
  return  ret;
}


void QEpicsPV::updateValue(const QVariant & data){

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPV DEBUG: UPD" << this << isConnected() << pv() << get() << data << getEnum();

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


void QEpicsPV::updateConnection() {
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPV DEBUG: CON" << this << pv() << isConnected();
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

const QStringList & QEpicsPV::getEnum() const {
  return theEnum;
}
