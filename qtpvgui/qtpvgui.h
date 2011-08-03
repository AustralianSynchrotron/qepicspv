#ifndef QEpicsPvGUI_H
#define QEpicsPvGUI_H

#include "qtpv.h"
#include "ui_qtpvgui.h"

#include <QWidget>


namespace Ui {
    class QEpicsPvGUI;
}

class QEpicsPvGUI : public QEpicsPv {
  Q_OBJECT;

private:
  QWidget * theWidget;          ///< Main widget.
  Ui::QEpicsPvGUI *ui;

  void initialize();

public:

  QEpicsPvGUI(QWidget *parent = 0);
  QEpicsPvGUI(const QString & _pvName, QWidget *parent = 0);
  ~QEpicsPvGUI();

  inline Ui::QEpicsPvGUI * basicUI() {return ui;}

private slots:

  void onConnectionChange(bool con);
  void onValueChange(const QVariant & val);

  void onSet();

};

#endif // QEpicsPvGUI_H
