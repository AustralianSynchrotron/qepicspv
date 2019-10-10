#ifndef QTPVWIDGETS_PRIVATE_H
#define QTPVWIDGETS_PRIVATE_H


class Protector : public QWidget {
public:
  Protector(QWidget * parent);
protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
};

#endif // QTPVWIDGETS_PRIVATE_H

