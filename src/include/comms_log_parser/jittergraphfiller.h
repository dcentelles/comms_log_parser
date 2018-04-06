#ifndef JITTERPLOTWINDOW_H
#define JITTERPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <qcustomplot.h>
#include <comms_log_parser/graphfiller.h>

class JitterGraphFiller : public GraphFiller {

public:
  explicit JitterGraphFiller();
  QVector<QCPGraphData> fillGraphData(const QList<DataRegisterPtr> & regs);

};

#endif // JITTERPLOTWINDOW_H
