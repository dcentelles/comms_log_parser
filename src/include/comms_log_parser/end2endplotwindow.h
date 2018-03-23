#ifndef END2ENDPLOTWINDOW_H
#define END2ENDPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/plotwindow.h>
#include <qcustomplot.h>

namespace Ui {
class End2EndPlotWindow;
}

class End2EndPlotWindow : public PlotWindow {
  Q_OBJECT

public:
  explicit End2EndPlotWindow(QWidget *parent = 0);
  QVector<QCPGraphData> fillGraphData(const QDateTime & relativeTo, const QList<DataRegisterPtr> & regs);
};

#endif // END2ENDPLOTWINDOW_H
