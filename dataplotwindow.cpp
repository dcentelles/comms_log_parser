#include "dataplotwindow.h"
#include "ui_dataplotwindow.h"

DataPlotWindow::DataPlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataPlotWindow)
{
    ui->setupUi(this);
}

DataPlotWindow::~DataPlotWindow()
{
    delete ui;
}

void DataPlotWindow::Plot(QList<DataRegisterPtr> txPdus,
          QList<DataRegisterPtr> rxPdus,
          QList<DataRegisterPtr> errors,
          QDateTime tini, QDateTime tend)
{


}
