#ifndef GRAPHFILLER_H
#define GRAPHFILLER_H

#include <comms_log_parser/dataregister.h>
#include <qcustomplot.h>
#include <memory>

class GraphFiller {

public:
  virtual QVector<QCPGraphData>
  fillGraphData(const QList<DataRegisterPtr> &regs) = 0;
};

typedef std::shared_ptr<GraphFiller> GraphFillerPtr ;
#endif
