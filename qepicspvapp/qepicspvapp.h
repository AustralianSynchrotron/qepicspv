#ifndef QEpicsPvAPP_H
#define QEpicsPvAPP_H

#include "qepicspvgui.h"
#include "ui_qepicspvapp.h"

#include <QtGui/QMainWindow>

namespace Ui {
    class QEpicsPvapp;
}

class QEpicsPvapp : public QMainWindow
{
    Q_OBJECT

public:
    explicit QEpicsPvapp(QWidget *parent = 0);
    ~QEpicsPvapp();

private:
    Ui::QEpicsPvapp *ui;
};

#endif // QEpicsPvAPP_H
