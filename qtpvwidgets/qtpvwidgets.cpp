
#include "qtpvwidgets.h"


void QMLineEdit::focusInEvent(QFocusEvent * event){
  QLineEdit::focusInEvent(event);
  oldtext = text();
}

void QMLineEdit::focusOutEvent(QFocusEvent * event){
  QLineEdit::focusOutEvent(event);
  emit escaped();
}

void QMLineEdit::keyPressEvent( QKeyEvent * event ){
  QLineEdit::keyPressEvent(event);
  int key = event->key();
  if ( key == Qt::Key_Enter || key == Qt::Key_Return )
    emit editingFinished( oldtext = text() );
  else if ( key == Qt::Key_Escape )
    emit escaped();
}

QMLineEdit::QMLineEdit(QWidget * parent) :
  QLineEdit(parent)
{
  connect(this, SIGNAL(escaped()), SLOT(restore()));
}









QMDoubleSpinBox::QMDoubleSpinBox(QWidget * parent) :
  QDoubleSpinBox(parent),
  validateMe(true)
{
  setKeyboardTracking(false);
  connect(this, SIGNAL(valueChanged(double)), SLOT(recalculateStep(double)));
  connect(this, SIGNAL(escaped()), SLOT(restore()));
  connect(lineEdit(), SIGNAL(cursorPositionChanged(int,int)),
          SLOT(correctPosition(int,int)));
  recalculateStep(value());
}

void QMDoubleSpinBox::focusInEvent(QFocusEvent * event){
  validateMe = false;
  QDoubleSpinBox::focusInEvent(event);
  selectAll();
  oldvalue = value();
}


void QMDoubleSpinBox::focusOutEvent(QFocusEvent * event){
  validateMe = true;
  QDoubleSpinBox::focusOutEvent(event);
  emit escaped();
}


void QMDoubleSpinBox::keyPressEvent( QKeyEvent * event ){
  int key = event->key();
  if ( key == Qt::Key_Enter || key == Qt::Key_Return ) {
    validateMe = true;
    interpretText();
    validateMe = false;
    emit valueEdited(oldvalue=value());
  } else if ( key == Qt::Key_Escape )
    emit escaped();
  QDoubleSpinBox::keyPressEvent(event);
}




void QMDoubleSpinBox::correctPosition(int , int newPos) {
  QLineEdit * ledt = lineEdit();
  int maxPos = ledt->text().length() - suffix().length();
  if (newPos > maxPos) {
    int selSt = ledt->selectionStart();
    ledt->setCursorPosition(maxPos);
    if ( selSt >= 0 ) // needed here because setCursorPosition deselects.
      ledt->setSelection(selSt,maxPos-selSt);
  }
}


/// Reduces the meaningless zeros from the string representing a double value.
/// @param value value to represent as the string
/// @return String representation of the value.
QString QMDoubleSpinBox::textFromValue ( double value ) const {
  QString prn = QString::number(value, 'f', decimals());
  if (prn.contains('.')) {
    while (prn.endsWith('0'))
      prn.chop(1);
    if (prn.endsWith('.'))
      prn.chop(1);
  }
  return prn;
}

void QMDoubleSpinBox::recalculateStep(double val){
  int power;
  QString vals = QString::number(val,'e');
  power = vals.remove(0, vals.indexOf('e') + 1).toInt();
  if ( -power >= decimals() || val == 0.0 )
    power = -decimals() + 1 ;
  double step = pow(10, power - 1);
  if ( step != singleStep() )
    setSingleStep(step);
}

QValidator::State	QMDoubleSpinBox::validate ( QString & text, int & pos ) const {
  return validateMe ?
        QDoubleSpinBox::validate(text, pos) :
        QValidator::Acceptable;
}

void QMDoubleSpinBox::setValue(double val) {
  oldvalue = val;
  if ( ! hasFocus() || ! isEnabled() )
    QDoubleSpinBox::setValue(val);
}







QMSpinBox::QMSpinBox(QWidget * parent)  :
  QSpinBox(parent),
  validateMe(true)
{
  setKeyboardTracking(false);
  connect(this, SIGNAL(escaped()), SLOT(restore()));
  connect(this->lineEdit(), SIGNAL(cursorPositionChanged(int,int)),
          SLOT(correctPosition(int,int)));
}

void QMSpinBox::focusInEvent(QFocusEvent * event){
  validateMe = false;
  QSpinBox::focusInEvent(event);
  selectAll();
  oldvalue = value();
}

void QMSpinBox::focusOutEvent(QFocusEvent * event){
  validateMe = true;
  QSpinBox::focusOutEvent(event);
  emit escaped();
}

void QMSpinBox::keyPressEvent( QKeyEvent * event ){
  int key = event->key();
  if ( key == Qt::Key_Enter || key == Qt::Key_Return ) {
    validateMe = true;
    interpretText();
    validateMe = false;
    emit valueEdited(oldvalue=value());
  } else if ( key == Qt::Key_Escape )
    emit escaped();
  QSpinBox::keyPressEvent(event);
}

void QMSpinBox::correctPosition(int , int newPos) {
  QLineEdit * ledt = lineEdit();
  int maxPos = ledt->text().length() - suffix().length();
  if (newPos > maxPos) {
    int selSt = ledt->selectionStart();
    ledt->setCursorPosition(maxPos);
    if ( selSt >= 0 ) // needed here because setCursorPosition deselects.
      ledt->setSelection(selSt,maxPos-selSt);
  }
}

QValidator::State	QMSpinBox::validate ( QString & text, int & pos ) const {
  return validateMe ?
        QSpinBox::validate(text, pos) :
        QValidator::Acceptable;
}

void QMSpinBox::setValue(int val) {
  oldvalue = val;
  if ( ! hasFocus() || ! isEnabled() )
    QSpinBox::setValue(val);
}









void QMComboBox::focusInEvent(QFocusEvent * event){
  QComboBox::focusInEvent(event);
  oldvalue = currentIndex();
}

void QMComboBox::focusOutEvent(QFocusEvent * event){
  QComboBox::focusOutEvent(event);
  emit escaped();
}

void QMComboBox::keyPressEvent( QKeyEvent * event ){
  QComboBox::keyPressEvent(event);
  int key = event->key();
  if ( key == Qt::Key_Enter || key == Qt::Key_Return ) {
    emit textEdited(currentText());
    emit indexEdited(currentIndex());
    oldvalue = currentIndex();
  } else if ( key == Qt::Key_Escape )
    emit escaped();
}

QMComboBox::QMComboBox(QWidget * parent) :
  QComboBox(parent)
{
  connect(this, SIGNAL(escaped()), SLOT(restore()));
}


void QMComboBox::setIndexFromText(const QString & text) {
  int idx = findText(text);
  if (idx >= 0)
    setCurrentIndex( findText(text) );
}











void QVariantLabel::setParam(int param) {
  m_param=param;
  retext();
}

int QVariantLabel::param() const {
  return m_param;
}

void QVariantLabel::retext() {
  QString nText;
  if (!variable.isValid())
    return;
  switch (variable.type()) {
    case QVariant::Double :
      nText =
          m_prefix +
          QString::number(variable.toDouble(), m_format, m_param) +
          m_suffix;
      break;
    case QVariant::LongLong :
      nText =
          m_prefix +
          QString::number(variable.toLongLong(), m_param) +
          m_suffix;
      break;
    case QVariant::ULongLong:
      nText =
          m_prefix +
          QString::number(variable.toLongLong(), m_param) +
          m_suffix;
      break;
    default :
      nText = variable.toString();
      break;
  }
  QLabel::setText(nText);
  emit somethingChanged(text());
}

void QVariantLabel::setDecimals(int prec) {
  setParam(prec);
}

void QVariantLabel::setBase(int base) {
  setParam(base);
}

char QVariantLabel::format() const {
  return m_format;
}


void QVariantLabel::setFormat(char f){
  m_format=f;
  retext();
}

void QVariantLabel::setText(const QString & s) {
  variable=s;
  QLabel::setText(s);
  emit somethingChanged(text());
}

void QVariantLabel::setValue(double n) {
  variable=n;
  retext();
}

void QVariantLabel::setValue(qlonglong n) {
  variable=n;
  retext();
}

void QVariantLabel::setValue(qulonglong n) {
  variable=n;
  retext();
}

void QVariantLabel::setVariant(QVariant v) {
  variable=v;
  retext();
}

void QVariantLabel::setPrefix(const QString &prefix) {
  m_prefix=prefix;
  retext();
}

void QVariantLabel::setSuffix(const QString &suffix) {
  m_suffix=suffix;
  retext();
}

const QString & QVariantLabel::suffix() const {
  return m_suffix;
}

const QString & QVariantLabel::prefix() const {
  return m_prefix;
}







