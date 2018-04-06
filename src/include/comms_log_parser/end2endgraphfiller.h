#ifndef END2ENDPLOTWINDOW_H
#define END2ENDPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/graphfiller.h>
#include <qcustomplot.h>

class End2EndGraphFiller : public GraphFiller {

public:
  End2EndGraphFiller();
  QVector<QCPGraphData> fillGraphData(const QList<DataRegisterPtr> & regs);
};

#endif // END2ENDPLOTWINDOW_H
