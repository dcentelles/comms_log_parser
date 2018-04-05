#ifndef DISTANCEPLOTWINDOW_H
#define DISTANCEPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/plotwindow.h>
#include <qcustomplot.h>

namespace Ui {
class DistancePlotWindow;
}

class DistancePlotWindow : public PlotWindow {
  Q_OBJECT

public:
  explicit DistancePlotWindow(QWidget *parent = 0);
  QVector<QCPGraphData> fillGraphData(const QDateTime & relativeTo, const QList<DataRegisterPtr> & regs);
};

#endif // END2ENDPLOTWINDOW_H
