#include "qepicspv.h"
#include "src/QCaObject.h"

#include <QTime>
#include <QCoreApplication>

using namespace qcaobject;

const QVariant QEpicsPV::badData = QVariant();

const bool QEpicsPV::inited = QEpicsPV::init();

bool QEpicsPV::init() {
  qRegisterMetaType<QCaConnectionInfo>("QCaConnectionInfo&");
  qRegisterMetaType<QCaAlarmInfo>("QCaAlarmInfo&");
  qRegisterMetaType<QCaDateTime>("QCaDateTime&");
  qRegisterMetaType<QVariant>("QVariant&");
  qRegisterMetaType<QVariant>("QVariant");
  return true;
}

QEpicsPV::QEpicsPV(const QString & _pvName, QObject *parent) :
    QObject(parent),
    qCaField(0),
    pvName(_pvName),
    lastData(),
    updated(false),
    theEnum()
{
  setPV(pvName);
}

QEpicsPV::QEpicsPV(QObject *parent) :
    QObject(parent),
    qCaField(0),
    pvName(),
    lastData(),
    updated(false),
    theEnum()
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
  if (_pvName.isEmpty())
    return badData;
  QEpicsPV * tpv = new QEpicsPV(_pvName);
  QVariant ret = tpv->getReady(delay);
  delete tpv;
  return  ret;
}

const QVariant & QEpicsPV::set(const QVariant & value, int delay) {

  if ( ! isConnected() )
    return badData ;

  if (delay >= 0)
    needUpdated();

  if (getEnum().size() && value.canConvert<int>())
    ((QCaObject *) qCaField) -> writeData(getEnum()[value.toInt()]);
  else
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

  updated = true;
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
