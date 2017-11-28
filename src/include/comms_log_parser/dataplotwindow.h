#ifndef DATAPLOTWINDOW_H
#define DATAPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
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
             QDateTime tini, QDateTime tend,
             const QString & txtitle,
             const QString & rxtitle,
             const QString & errtitle);

   void PlotOver(QList<DataRegisterPtr> txPdus,
             QList<DataRegisterPtr> rxPdus,
             QList<DataRegisterPtr> errors,
             QDateTime tini, QDateTime tend,
             const QString & txtitle,
             const QString & rxtitle,
             const QString & errtitle);

private slots:
   void on_tickStepSpinBox_valueChanged(int arg1);

   void on_blockXToggle_toggled(bool checked);

   void on_blockYToggle_toggled(bool checked);

   void on_saveAsPDFButton_clicked();

private:
    void setDRsToTimeGraph(
                          QCPGraph * graph,
                          QList<DataRegisterPtr> drs);
    void updateZoomSettingsFromUi();
    void DrawDRsLinksToTimeGraph(
            QCustomPlot * plot,
            QList<DataRegisterPtr> pdus
            );

    Ui::DataPlotWindow *ui;
};

#endif // DATAPLOTWINDOW_H
