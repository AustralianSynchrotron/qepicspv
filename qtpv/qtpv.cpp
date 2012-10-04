#include "qtpv.h"
#include "src/QCaObject.h"

#include <QTime>
#include <QCoreApplication>
#include <QDebug>




bool qtWait(const QList<ObjSig> & osS, int delay_msec) {

  QEventLoop q;
  foreach(ObjSig os, osS)
    QObject::connect(os.sender, os.signal, &q, SLOT(quit()));

  QTimer * tT = 0;
  if (delay_msec>0) {
    tT = new QTimer;
    tT->setSingleShot(true);
    QObject::connect(tT, SIGNAL(timeout()), &q, SLOT(quit()));
    tT->start(delay_msec);
  }

  q.exec();

  bool ret = true;
  if (tT) {
    ret = tT->isActive();
    delete tT;
  }
  return ret;

}

bool qtWait(const QObject * sender, const char * signal, int delay_msec) {
  QList<ObjSig> osS;
  osS << ObjSig(sender,signal);
  return qtWait(osS,delay_msec);
}

bool qtWait(int delay_msec){

  if (delay_msec <= 0)
    return true;

  QTimer * tT = 0;
  tT = new QTimer;
  tT->setSingleShot(true);
  QEventLoop q;

  QObject::connect(tT, SIGNAL(timeout()), &q, SLOT(quit()));

  tT->start(delay_msec);
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
  updated(false)
{
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: INI" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") <<
                this << _pvName;
  installEventFilter(this);
  setPV(pvName);
}

QEpicsPv::QEpicsPv(QObject *parent) :
  QObject(parent),
  qCaField(0),
  pvName(),
  lastData(),
  updated(false)
{
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: INI" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
             << this;
  installEventFilter(this);
}


QEpicsPv::~QEpicsPv(){
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: DEL" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
             << this << isConnected() << pv();
  if ( ! pv().isEmpty() )
    setPV();
}

bool QEpicsPv::eventFilter(QObject *obj, QEvent *event) {
  if ( event->type() == QEvent::ApplicationActivate )
    preSetPV();
  return QObject::eventFilter(obj, event);
}


void QEpicsPv::setPV(const QString & _pvName) {
  pvName = _pvName;
  setObjectName(pvName);
  // to guarantee that the preSetPV is not called before the QCoreApplication::exec()
  if ( QCoreApplication::startingUp() )
    QCoreApplication::postEvent(this, new QEvent(QEvent::ApplicationActivate));
  else
    QTimer::singleShot(0,this,SLOT(preSetPV()));
}


void QEpicsPv::preSetPV() {

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: SPV" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
             << this << pvName ;

  if (qCaField) {
    qCaField->deleteChannel();
    delete qCaField;
    qCaField = 0;
  }
  updateConnection();

  if ( ! pvName.isEmpty() ) {

    qCaField = new QCaObject(pvName, QCoreApplication::instance());

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

  }

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

const QVariant & QEpicsPv::getUpdated(int delay_msec) const {

  if ( ! qCaField )
    return badData;
  if ( updated )
    return lastData;

  QList<ObjSig> osS;
  osS
      << ObjSig(this, SIGNAL(valueUpdated(QVariant)))
      << ObjSig(this, SIGNAL(connectionChanged(bool)));
  qtWait(osS, delay_msec);

  QCoreApplication::processEvents(); // needed to process updateValue
  return ( ! isConnected() || ! updated )
      ? badData : lastData;

}



const QVariant & QEpicsPv::getConnected(int delay_msec) const {
  if ( ! qCaField )
    return badData;
  if ( ! isConnected() )
    qtWait(this, SIGNAL(connected()), delay_msec);
  return lastData;
}




QVariant QEpicsPv::get(const QString & _pvName, int delay_msec) {
  if ( _pvName.isEmpty() )
    return badData;
  QEpicsPv tpv(_pvName);
  QCoreApplication::processEvents();
  return tpv.getConnected(delay_msec);
}

const QVariant & QEpicsPv::set(const QVariant & value, int delay_msec) {

  emit valueUpdated(get());

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: SET" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
             << this << isConnected() << pv() << get() << value << getEnum();

  if ( ! isConnected() || ! value.isValid() )
    return badData ;

  if (delay_msec >= 0)
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
      qCaField->writeData(val);
    }

  } else if ( get().type() == QVariant::List && value.type() == QVariant::ByteArray ) {

    qCaField->writeData(value);

  } else if ( get().type() != value.type()  && ! value.canConvert(get().type()) ) {

    qDebug() << "ERROR in QEpicsPv! Could not convert type QVariant from \"" << value.typeName()
             << "\" to \"" << get().typeName() << "\" to set the PV" << pv();
    return badData;

  } else {

    qCaField->writeData(value);

  }

  return delay_msec >= 0  ?  getUpdated(delay_msec)  :  get();

}



QVariant QEpicsPv::set(const QString & _pvName, const QVariant & value, int delay_msec) {
  if (_pvName.isEmpty())
    return badData;
  QEpicsPv tpv(_pvName);
  QCoreApplication::processEvents();
  return tpv.getConnected().isValid()  ?  tpv.set(value, delay_msec)  :  badData;
}


void QEpicsPv::updateValue(const QVariant & data){

  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: UPD" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
             << this << isConnected() << pv() << get() << data << getEnum();

  if ( ! qCaField || ! data.isValid() )
    return;

  updated = true;
  bool firstRead = ! lastData.isValid();
  bool changed = firstRead || (lastData != data);
  lastData = data;

  if (firstRead) {
    emit connected();
    emit connectionChanged(true);
  }
  if (changed)
    emit valueChanged(lastData);
  emit valueUpdated(lastData);

}


void QEpicsPv::updateConnection() {

  bool con =  isConnected();
  if ( debugLevel > 0 )
    qDebug() << "QEpicsPv DEBUG: CON" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
             << this << pv() << con ;

  if (con) {
    emit connected();
  } else {
    updated=false;
    lastData = badData;
    emit disconnected();
  }
  emit connectionChanged(con);

}

const QStringList QEpicsPv::getEnum() const {
  return qCaField->getEnumerations();
}
