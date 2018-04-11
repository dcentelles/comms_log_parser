#ifndef DATAPLOTWINDOW_H
#define DATAPLOTWINDOW_H

#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/datetimeplotwindow.h>
#include <qcustomplot.h>
#include <ui_datetimeplotwindow.h>

class DataPlotWindow : public DateTimePlotWindow
{
public:
    explicit DataPlotWindow(QWidget *parent = 0);
    ~DataPlotWindow();

  void Plot(QList<DataRegisterPtr> txPdus,
            QList<DataRegisterPtr> rxPdus,
            QList<DataRegisterPtr> propErrors,
            QList<DataRegisterPtr> colErrors,
            QList<DataRegisterPtr> multErrors,
            uint64_t tini, uint64_t tend,
            const QString & txtitle,
            const QString & rxtitle,
            const QString & errtitle);

  void PlotOver(QList<DataRegisterPtr> txPdus,
            QList<DataRegisterPtr> rxPdus,
            QList<DataRegisterPtr> propErrors,
            QList<DataRegisterPtr> colErrors,
            QList<DataRegisterPtr> multErrors,
            uint64_t tini, uint64_t tend,
            const QString & txtitle,
            const QString & rxtitle,
            const QString & errtitle);

   void Plot(QList<DataRegisterPtr> txPdus,
             QList<DataRegisterPtr> rxPdus,
             QList<DataRegisterPtr> errors,
             uint64_t tini, uint64_t tend,
             const QString & txtitle,
             const QString & rxtitle,
             const QString & errtitle);

   void PlotOver(QList<DataRegisterPtr> txPdus,
             QList<DataRegisterPtr> rxPdus,
             QList<DataRegisterPtr> errors,
             uint64_t tini, uint64_t tend,
             const QString & txtitle,
             const QString & rxtitle,
             const QString & errtitle);


private:
    void setDRsToTimeGraph(
                          QCPGraph * graph,
                          QList<DataRegisterPtr> drs);
    void DrawDRsLinksToTimeGraph(
            QCustomPlot * plot,
            QList<DataRegisterPtr> pdus
            );
};

#endif // DATAPLOTWINDOW_H
