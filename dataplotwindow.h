#ifndef DATAPLOTWINDOW_H
#define DATAPLOTWINDOW_H

#include <QMainWindow>
#include <dataregister.h>

namespace Ui {
class DataPlotWindow;
}

class DataPlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataPlotWindow(QWidget *parent = 0);
    ~DataPlotWindow();

   void Plot(QList<DataRegisterPtr> txPdus,
             QList<DataRegisterPtr> rxPdus,
             QList<DataRegisterPtr> errors,
             QDateTime tini, QDateTime tend);


private:
    Ui::DataPlotWindow *ui;
};

#endif // DATAPLOTWINDOW_H
