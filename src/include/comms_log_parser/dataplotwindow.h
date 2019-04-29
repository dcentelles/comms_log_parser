#ifndef DATAPLOTWINDOW_H
#define DATAPLOTWINDOW_H

#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/datetimeplotwindow.h>
#include <qcustomplot.h>
#include <ui_datetimeplotwindow.h>

class DataPlotWindow : public DateTimePlotWindow {
public:
  explicit DataPlotWindow(QWidget *parent = 0);
  ~DataPlotWindow();

  void Plot(QList<DataRegisterPtr> txPdus, QList<DataRegisterPtr> rxPdus,
            QList<DataRegisterPtr> propErrors, QList<DataRegisterPtr> colErrors,
            QList<DataRegisterPtr> multErrors, uint64_t tini, uint64_t tend,
            const QString &txtitle, const QString &rxtitle,
            const QString &errtitle, bool plotLinks = true,
            bool yTicksTextLabels = true);

  void PlotOver(QList<DataRegisterPtr> txPdus, QList<DataRegisterPtr> rxPdus,
                QList<DataRegisterPtr> propErrors,
                QList<DataRegisterPtr> colErrors,
                QList<DataRegisterPtr> multErrors, uint64_t tini, uint64_t tend,
                const QString &txtitle, const QString &rxtitle,
                const QString &errtitle, bool plotLinks = true,
                bool yTicksTextLabels = true);

  void Plot(QList<DataRegisterPtr> txPdus, QList<DataRegisterPtr> rxPdus,
            QList<DataRegisterPtr> errors, uint64_t tini, uint64_t tend,
            const QString &txtitle, const QString &rxtitle,
            const QString &errtitle, bool plotLinks = true, bool addErr = true,
            bool yTicksTextLabels = true);

  void PlotOver(QList<DataRegisterPtr> txPdus, QList<DataRegisterPtr> rxPdus,
                QList<DataRegisterPtr> errors, uint64_t tini, uint64_t tend,
                const QString &txtitle, const QString &rxtitle,
                const QString &errtitle, bool plotLinks = true,
                bool addErr = true, bool yTicksTextLabels = true);

private:
  void setDRsToTimeGraph(QCPGraph *graph, QList<DataRegisterPtr> drs,
                         bool yTicksTextLabels, double newYValue = 0);
  void DrawDRsLinksToTimeGraph(QCustomPlot *plot, QList<DataRegisterPtr> pdus,
                               bool yTicksTextLabels = true);
  int plotCount = 0;
};

typedef std::shared_ptr<DataPlotWindow> DataPlotWindowPtr;

#endif // DATAPLOTWINDOW_H
