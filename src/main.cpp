#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "EDA Desktop Application v" << EDA_VERSION;
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
