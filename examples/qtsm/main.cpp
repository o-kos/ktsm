#include <QCoreApplication>
#include <QSharedMemory>
#include <QTimer>

#include "../main.hpp"

int sm_main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QTimer::singleShot(0, &a, &QCoreApplication::quit);

    return a.exec();
}
