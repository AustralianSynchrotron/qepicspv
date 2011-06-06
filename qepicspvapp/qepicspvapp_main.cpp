#include <QtGui/QApplication>
#include "qepicspvapp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QEpicsPvapp w;
    w.show();

    return a.exec();
}
