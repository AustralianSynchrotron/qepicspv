#ifndef QEPICSPV_GUIELEMENTS_H
#define QEPICSPV_GUIELEMENTS_H




#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>

#include <math.h>


class QMLineEdit : public QLineEdit {
  Q_OBJECT;
private:

  QString oldtext;

  void focusInEvent(QFocusEvent * event);
  void focusOutEvent(QFocusEvent * event);
  void keyPressEvent(QKeyEvent * event);

public:

  QMLineEdit(QWidget * parent = 0);

signals:

  void escaped();
  void editingFinished(const QString & text);

public slots:
  inline void	setText ( const QString & text ) { QLineEdit::setText(oldtext=text); }

private slots:
  inline void restore() { setText(oldtext); }

};






/// Widget which modifies the behaviour of the QDoubleSpinBox.
///
/// The modifications:
/// 1) Setting new value must be confirmed with the press of the Enter key.
///    If the spinbox loses the focus without confirmation, the previous value
///    is restored.
/// 2) The meaningless zeros after the decimal point are erased (f.e. 0.0300 is reduced to 0.03).
/// 3) Step of the spinbox is recalculated from the current value as 1/10 of the most significant
///    order.
///
class QMDoubleSpinBox : public QDoubleSpinBox {
  Q_OBJECT;
private:

  /// Stores the value in the spinbox while the new value is being edited.
  /// Needed to restore the old value if the new one was not confirmed with the
  /// Enter press.
  double oldvalue;

  bool validateMe;

  /// Reimplements the QDoubleSpinBox::focusInEvent().
  /// Updates the ::oldvalue and selects the text in the edit line.
  /// @param event focusing event.
  void focusInEvent(QFocusEvent * event);

  /// Reimplements the QDoubleSpinBox::focusOutEvent().
  /// @param event focusing event.
  void focusOutEvent(QFocusEvent * event);

  /// \brief Reimplements the QDoubleSpinBox::keyPressEvent().
  ///
  /// Updates the ::oldvalue with the edited on Enter pressed,
  /// escapes on Escape pressed.
  ///
  /// @param event key press event.
  ///
  void keyPressEvent( QKeyEvent * event );

public:

  /// Constructor
  /// @param paren object's parent.
  QMDoubleSpinBox(QWidget * parent = 0);

  /// Reduces the meaningless zeros from the string representing a double value.
  /// @param value value to represent as the string
  /// @return String representation of the value.
  QString textFromValue ( double value ) const ;

  virtual QValidator::State	validate ( QString & text, int & pos ) const;

signals:

  /// Emitted on the confirmed value change.
  /// @param double new value
  void valueEdited(double);

  /// Emitted when the focus is lost or Escape pressed.
  void escaped();

public slots:

  /// Slot made from the QDoubleSpinBox::setMinimum().
  /// @param min new minimum.
  inline void setMin(double min) {setMinimum(min);}

  /// Slot made from the QDoubleSpinBox::setMaximum().
  /// @param max new maximum.
  inline void setMax(double max) {setMaximum(max);}

  /// Slot made from the QDoubleSpinBox::setDecimals().
  /// @param prec new precision.
  inline void setPrec(int prec) {setDecimals(prec);}

  inline void	setValue ( double val ) { QDoubleSpinBox::setValue( oldvalue = val ) ; }

private slots:

  /// Calculates and sets new single step of the spinbox:
  /// 1/10 of the current value's most significant order.
  /// @param val current value of the spinbox.
  void recalculateStep(double val);

  /// Restores the ::oldvalue (on escape).
  inline void restore() { setValue(oldvalue); }

  void correctPosition(int , int newPos);

};







/// Widget which modifies the behaviour of the QSpinBox.
///
/// Setting new value must be confirmed with the press of the Enter key.
/// If the spinbox loses the focus without confirmation, the previous value
/// is restored.
///
class QMSpinBox : public QSpinBox {
  Q_OBJECT;
private:

  /// Stores the value in the spinbox while the new value is being edited.
  /// Needed to restore the old value if the new one was not confirmed with the
  /// Enter press.
  int oldvalue;

  bool validateMe;

  /// Reimplements the QSpinBox::focusInEvent().
  /// Updates the ::oldvalue and selects the text in the edit line.
  /// @param event focusing event.
  void focusInEvent(QFocusEvent * event);

  /// Reimplements the QSpinBox::focusOutEvent().
  /// @param event focusing event.
  void focusOutEvent(QFocusEvent * event);

  /// \brief Reimplements the QSpinBox::keyPressEvent().
  ///
  /// Updates the ::oldvalue with the edited on Enter pressed,
  /// escapes on Escape pressed.
  ///
  /// @param event key press event.
  ///
  void keyPressEvent( QKeyEvent * event );

public:

  /// Constructor
  /// @param paren object's parent.
  QMSpinBox(QWidget * parent = 0);

  virtual QValidator::State	validate ( QString & text, int & pos ) const;

signals:

  /// Emitted on the confirmed value change.
  /// @param double new value
  void valueEdited(int);

  /// Emitted when the focus is lost or Escape pressed.
  void escaped();

public slots:

  /// Slot made from the QSpinBox::setMinimum().
  /// @param min new minimum.
  inline void setMin(int min) {setMinimum(min);}

  /// Slot made from the QSpinBox::setMaximum().
  /// @param max new maximum.
  inline void setMax(int max) {setMaximum(max);}

  inline void	setValue ( int val ) { QSpinBox::setValue( oldvalue = val ) ; }

private slots:

  /// Restores the ::oldvalue (on escape).
  inline void restore() {setValue(oldvalue);}

  void correctPosition(int , int newPos);

};



/// Widget which modifies the behaviour of the QComboBox.
///
/// Setting new value must be confirmed with the press of the Enter key.
/// If the widget loses the focus without confirmation, the previous value
/// is restored.
///
class QMComboBox : public QComboBox {
  Q_OBJECT;

private:

  /// Stores the value in the spinbox while the new value is being edited.
  /// Needed to restore the old value if the new one was not confirmed with the
  /// Enter press.
  int oldvalue;

  /// Reimplements the QComboBox::focusInEvent().
  /// Updates the ::oldvalue and selects the text in the edit line.
  /// @param event focusing event.
  void focusInEvent(QFocusEvent * event);

  /// Reimplements the QComboBox::focusOutEvent().
  /// @param event focusing event.
  void focusOutEvent(QFocusEvent * event);

  /// \brief Reimplements the QComboBox::keyPressEvent().
  ///
  /// Updates the ::oldvalue with the edited on Enter pressed,
  /// escapes on Escape pressed.
  ///
  /// @param event key press event.
  ///
  void keyPressEvent( QKeyEvent * event );

public:

  /// Constructor
  /// @param paren object's parent.
  QMComboBox(QWidget * parent = 0);

signals:

  /// Emitted on the confirmed value change.
  /// @param double new value
  void textEdited(QString);

  /// Emitted when the focus is lost or Escape pressed.
  void escaped();

public slots:

  /// Slot made from the QComboBox::setCurrentIndex().
  /// @param text text to search in the combobox.
  void setIndexFromText(const QString & text);

private slots:

  /// Restores the ::oldvalue (on escape).
  inline void restore() { setCurrentIndex(oldvalue); }

};





#endif // QEPICSPV_GUIELEMENTS_H
