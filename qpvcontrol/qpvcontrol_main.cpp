#include <QApplication>
#include "qpvcontrol.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QEpicsPvApp w;
    w.show();

    return a.exec();
}
