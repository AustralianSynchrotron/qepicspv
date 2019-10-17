%Module pyqtpvwidgets

%Import QtCore/QtCoremod.sip
%Import QtWidgets/QtWidgetsmod.sip


class QMLineEdit : public QLineEdit {
  %TypeHeaderCode
  #include "qtpvwidgets.h"
  %End

public:
  QMLineEdit(QWidget * parent = 0);

signals:
  void escaped();
  void editingFinished(const QString & text);

public slots:
  void setText (const QString & text);

private:
  void focusInEvent(QFocusEvent * event);
  void focusOutEvent(QFocusEvent * event);
  void keyPressEvent(QKeyEvent * event);

};


class QMDoubleSpinBox : public QDoubleSpinBox {
  %TypeHeaderCode
  #include "qtpvwidgets.h"
  %End

public:
  QMDoubleSpinBox(QWidget * parent = 0);
  QString textFromValue (double value) const ;
  virtual QValidator::State validate (QString & text, int & pos) const;
  void setConfirmationRequired(bool req=true);

signals:
  void valueEdited(double);
  void entered();
  void escaped();

public slots:
  void setMin(double min);
  void setMax(double max);
  void setPrec(int prec);
  void setValue(double val);

private:
  void focusInEvent(QFocusEvent * event);
  void focusOutEvent(QFocusEvent * event);
  void keyPressEvent(QKeyEvent * event);

};


class QMSpinBox : public QSpinBox {
  %TypeHeaderCode
  #include "qtpvwidgets.h"
  %End

public:
  QMSpinBox(QWidget * parent = 0);
  virtual QValidator::State validate ( QString & text, int & pos ) const;
  void setConfirmationRequired(bool req=true);

signals:
  void valueEdited(int);
  void entered();
  void escaped();

public slots:
  void setMin(int min);
  void setMax(int max);
  void setValue(int val);

private:
  void focusInEvent(QFocusEvent * event);
  void focusOutEvent(QFocusEvent * event);
  void keyPressEvent(QKeyEvent * event);

};



class QMComboBox : public QComboBox {
  %TypeHeaderCode
  #include "qtpvwidgets.h"
  %End

public:

  QMComboBox(QWidget * parent = 0);

signals:
  void textEdited(QString);
  void indexEdited(int);
  void escaped();

public slots:
  void setIndexFromText(const QString & text);

private:
  void focusInEvent(QFocusEvent * event);
  void focusOutEvent(QFocusEvent * event);
  void keyPressEvent( QKeyEvent * event );

};


class QVariantLabel : public QLabel {
  %TypeHeaderCode
  #include "qtpvwidgets.h"
  %End

public:
  QVariantLabel( QWidget * parent /TransferThis/ =0, Qt::WindowFlags f=0);
  QVariantLabel( const QString & text, QWidget * parent /TransferThis/ =0, Qt::WindowFlags f=0);
  void  setBase(int base);
  void  setFormat(char f);
  const QString & suffix() const;
  const QString & prefix() const;
  char format() const;

public slots:
  void setText(const QString & s);
  void setValue(const QString & s);
  void setValue(double n);
  void setValue(qlonglong n);
  void setValue(qulonglong n);
  void setValue(QVariant v);
  void setVariant(QVariant v);
  void setDecimals(int prec);
  void setPrefix(const QString &prefix);
  void setSuffix(const QString &suffix);

signals:
  void somethingChanged(const QString &);

};




