#ifndef QEpicsPvAPP_H
#define QEpicsPvAPP_H

#include "qtpvgui.h"
#include "ui_qpvcontrol.h"

#include <QtGui/QMainWindow>
#include <QFile>

namespace Ui {
class QEpicsPvApp;
}

class QEpicsPvApp : public QMainWindow {
  Q_OBJECT;

public:
  QEpicsPvApp(QWidget *parent = 0);
  QEpicsPvApp(const QString & _pvsFile, QWidget *parent = 0);
  ~QEpicsPvApp();

  QList < QEpicsPvGUI * > pvsList() const;

public slots:

  QEpicsPvGUI * addPv(const QString & presetpv = "", bool noFileSave = false);
  void addPv(QEpicsPvGUI * _pv, bool noFileSave = false);
  void removePv(QEpicsPvGUI * _pv);
  void clear();

  void saveConfiguration(const QString & fileName);
  void loadConfiguration(const QString & fileName);


private:

  Ui::QEpicsPvApp *ui;

  static const QString configsSearchBaseDir;

  QHash< QMLineEdit*, QEpicsPvGUI* > pvs;
  QFile pvsFile;

private slots:

  void removeRow(int idx);
  void updatePvsFile();
  void resetHeader();

  void onSave();
  void onLoad();

};

#endif // QEpicsPvAPP_H
