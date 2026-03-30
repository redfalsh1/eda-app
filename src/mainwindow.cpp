#include "mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("EDA Desktop Application");
    setFixedSize(800, 600);
    
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    QLabel *label = new QLabel("Welcome to EDA Desktop Application");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    layout->addWidget(label);
}

MainWindow::~MainWindow()
{
}
