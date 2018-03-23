#ifndef JITTERPLOTWINDOW_H
#define JITTERPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <qcustomplot.h>
#include <comms_log_parser/plotwindow.h>

namespace Ui {
class JitterPlotWindow;
}

class JitterPlotWindow : public PlotWindow {
  Q_OBJECT

public:
  explicit JitterPlotWindow(QWidget *parent = 0);
  QVector<QCPGraphData> fillGraphData(const QDateTime & relativeTo, const QList<DataRegisterPtr> & regs);

};

#endif // JITTERPLOTWINDOW_H
