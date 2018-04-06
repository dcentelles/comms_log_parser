#ifndef DISTANCEPLOTWINDOW_H
#define DISTANCEPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/graphfiller.h>
#include <qcustomplot.h>
class DoubleGraphFiller : public GraphFiller {

public:
  explicit DoubleGraphFiller();
  QVector<QCPGraphData> fillGraphData(const QList<DataRegisterPtr> & regs);
};

#endif // END2ENDPLOTWINDOW_H
