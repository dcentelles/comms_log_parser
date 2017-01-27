#ifndef DATAPLOTWINDOW_H
#define DATAPLOTWINDOW_H

#include <QMainWindow>
#include <dataregister.h>
#include <qcustomplot.h>

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

private slots:
   void on_tickStepSpinBox_valueChanged(int arg1);

   void on_blockXToggle_toggled(bool checked);

   void on_blockYToggle_toggled(bool checked);

private:
    void setDRsToTimeGraph(
                          double msT0,
                          QCPGraph * graph,
                          QList<DataRegisterPtr> drs);
    void updateZoomSettingsFromUi();

    Ui::DataPlotWindow *ui;
};

#endif // DATAPLOTWINDOW_H
