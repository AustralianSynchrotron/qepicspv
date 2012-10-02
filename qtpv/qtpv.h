#ifndef QEpicsPv_H
#define QEpicsPv_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QVariant>
#include <QStringList>

struct ObjSig {
  const QObject * sender;
  const char * signal;
  inline ObjSig(const QObject * _sender, const char * _signal) :
    sender(_sender), signal(_signal) {};
};

bool qtWait(const QList<ObjSig> & osS, int delay_msec=0);
bool qtWait(const QObject * sender, const char * signal, int delay_msec=0);
bool qtWait(int delay_msec);




namespace qcaobject {
class QCaObject;
}

/// \brief Class representing an EPICS PV field.
///
/// A wrapper around the QCaObject designed to hide EPICS-related features
/// unimportant and unclear for the end-user. Also implements some frequently
/// used operations in a single methods.
///
class QEpicsPv : public QObject {
  Q_OBJECT;

private:

  /// Basic object representing the field.
  qcaobject::QCaObject * qCaField;

  /// The PV name of the field
  QString pvName;

  /// Last value of the field picked by the ::updateValue() slot.
  /// Is invalid if the pv is not connected or before first update.
  QVariant lastData;

  /// Invalid QVariant object to be returned by ::get(), ::getUpdated(),
  /// ::set(), etc.
  static const QVariant badData;

  /// Turns true whenever the value is updated in the ::updateValue() slot.
  /// Used in the ::needUpdated() / ::getUpdated() pair.
  mutable bool updated;


  /// Default delay in milliseconds used in the ::getUpdated(), ::getReady() and get() methods.
  static const int defaultDelay = 1000;

  /// \brief Initializes the environment. Automatically called on program start.
  ///
  /// Registers the QCaConnectionInfo, QCaAlarmInfo, QCaDateTime. Needed to use
  /// the types in queued signal and slot connections in ::setPV().
  ///
  /// @return always true.
  ///
  static bool init();

  /// Used to automatically run ::init() before any instance of the object is created.
  static const bool inited;

  /// debug level. 0 - no debug;
  static unsigned debugLevel;

protected:
  bool eventFilter(QObject *obj, QEvent *event);

public:

  /// Constructor.
  /// @param _pvName new PV name.
  /// @param parent Object's parent.
  QEpicsPv(const QString & _pvName, QObject *parent = 0);

  /// Constructor.
  /// @param parent Object's parent.
  QEpicsPv(QObject *parent = 0);

  /// Destructor
  ~QEpicsPv();

  /// Expected time gap between the PV is set and the update arrives.
  /// This number is an average of the measured time gaps, NOT the
  /// guaranteed.
  static const int expectedResponse=100;

  static void setDebugLevel(unsigned level = 0);

  /// Returns current value of the field.
  ///
  /// @return Latest value of the PV field or invalid object
  /// if the field is not connected or before first update.
  ///
  const QVariant & get() const;

  /// \brief Starts monitoring for the updates.
  ///
  /// Makes sence only in combination with further call to ::getUpdated().
  /// Ensures that the next call to ::getUpdated() method will return only after
  /// a new value of the PV is received.
  //
  void needUpdated() const;

  /// \brief Returns an updated value of the field.
  ///
  /// Same as ::get(), but ensures that the PV's value was updated since the last call to
  /// ::needUpdated() method or establishing the connection.
  ///
  /// @param delay Maximum waiting time in milliseconds. If zero, then unlimited waiting.
  ///
  /// @return Updated value of the field. If the value was not updated during
  ///  the specified delay then returns an invalid object.
  ///
  // WARNING: BUG
  // There is a bug which may cause the main application to crash
  // if this static member is called (directly or inderectly)
  // from whithin a constructor.
  // It has something to do with the threading.
  const QVariant & getUpdated(int delay_msec=defaultDelay) const;

  /// \brief Connection status.
  ///
  /// Note that this status is different from the QCaObject::isChannelConnected() :
  /// It is true if QCaObject::isChannelConnected() _and_ a first value update has happened.
  ///
  /// @return true if connected, false otherwise.
  ///
  bool isConnected() const;

  /// PV's enumeration.
  /// @return The enumeration of the PV field, or the empty list if the PV is not an enumeration.
  const QStringList getEnum() const;

  /// PV name.
  /// @return PV name.
  const QString & pv() const ;


  /// \brief Ensures that the object is ready to operate.
  ///
  /// Connection to the EPICS PV and first update takes certain time after the ::setPV() method
  /// or the constructor with name are called. This method waits for the PV to get connected and
  /// the value to be initialized before returning.
  ///
  /// @param delay Maximum waiting time in milliseconds. If zero, then unlimited waiting.
  /// Note that the actual waiting time can be twice as large as the delay: one delay to wait for the
  /// connection and one more to wait for the first update.
  ///
  /// @return Field's updated value. If the connection could not be established or the value was
  /// not updated then returns an invalid data.
  ///
  // WARNING: BUG
  // There is a bug which may cause the main application to crash
  // if this member is called (directly or inderectly) from whithin a constructor.
  // It has something to do with the threading.
  const QVariant & getConnected(int delay_msec=defaultDelay) const;

  /// \brief Static version of the ::getUpdated() method.
  ///
  /// Constructs new object of the QEpicsPv type, gets it ready and returns the value.
  /// Useful for the single shot readouts.
  ///
  /// @param _pvName PV name of the field.
  /// @param delay Maximum waiting time in milliseconds. If zero, then unlimited waiting.
  ///
  /// @return Value of the field. If the value was not updated during
  /// the specified delay then returns an invalid object.
  ///
  // WARNING: BUG
  // There is a bug which may cause the main application to crash
  // if this static member is called (directly or inderectly)
  // from whithin a constructor.
  // It has something to do with the threading.
  static QVariant get(const QString & _pvName, int delay_msec=defaultDelay);

  /// \brief Static version of the ::set() method.
  ///
  /// Constructs new object of the QEpicsPv type, gets it ready, sets the new value
  /// and returns the result of the ::set() method. Useful for the single shot write.
  ///
  /// @param _pvName PV name of the field.
  /// @param value Value to be set.
  /// @param delay Negative delay tells that the confirmation of the update is
  /// nor required (just puts the new value and returns).
  /// If non-negative, then sets the maximum time in milliseconds to wait for the update event before return.
  /// Zero delay sets unlimited waiting time. Note that the method does not confirm the updated value
  /// to be equal to the one set.
  ///
  /// @return Updated value or invalid object if could not confirm the update within
  /// the specified delay time.
  ///
  ///
  // WARNING: BUG
  // There is a bug which may cause the main application to crash
  // if this static member is called (directly or inderectly)
  // from whithin a constructor.
  // It has something to do with the threading.
  static QVariant set(const QString & _pvName, const QVariant & value, int delay_msec = -1);

public slots:

  /// \brief Sets new value for the field.
  ///
  /// @param value Value to be set.
  /// @param delay Negative delay tells that the confirmation of the update is
  /// nor required (just puts the new value and returns).
  /// If non-negative, then sets the maximum time in milliseconds to wait for the update event before return.
  /// Zero delay sets unlimited waiting time. Note that the method does not confirm the updated value
  /// to be equal to the one set.
  ///
  /// @return Last updated value or invalid object if could not confirm the update within
  /// the specified delay time.
  ///
  const QVariant & set(const QVariant & value, int delay = -1);

  /// \brief Sets new PV.
  ///
  /// First disconnects any previous PV (if any was connected) and
  /// connects to the new one (if non-empty).
  ///
  /// @param _pvName New PV name.
  ///
  void setPV(const QString & _pvName="");


private slots:

  /// Used to catch the QCaObject::dataChanged() signal from the ::qCaField.
  /// @param data new data.
  void updateValue(const QVariant & data);

  /// Used to catch QCaObject::connectionChanged() signal from the ::qCaField.
  void updateConnection();

  /// Actually does all the job to set the pv name.
  /// Is called from the setPv(...) via a 0-timed timer to
  /// guarantee that nothing happens before the QCoreApplication::exec()
  void preSetPV();


signals:

  /// Emitted whenever the connection status has changed.
  /// @param connected new connection status.
  void connectionChanged(bool connected);

  /// Emitted on successful connection.
  void connected();

  /// Emitted on disconnection, destruction and in ::setPV() method.
  void disconnected();

  /// Emitted whenever the field is updated with a value different from the old one.
  /// @param value new value.
  void valueChanged(const QVariant & value);

  /// Emitted whenever the field is updated (even if the new value is equal to the old one).
  /// @param value new value.
  void valueUpdated(const QVariant & value);

  void pvChanged(const QString & newpv);

};

#endif // QEpicsPv_H
