#include <comms_log_parser/doublegraphfiller.h>
#include <ui_end2endplotwindow.h>

DoubleGraphFiller::DoubleGraphFiller() {}

QVector<QCPGraphData>
DoubleGraphFiller::fillGraphData(const QList<DataRegisterPtr> &regs) {
  QVector<QCPGraphData> graphData(regs.count());

  for (int i = 0; i < graphData.count(); i++) {
    auto dr = regs[i];
    auto secSinceEpoch = dr->GetSecs();
    auto secs = secSinceEpoch; // - secsBegin;
    graphData[i].key = secs;
    graphData[i].value = dr->GetDoubleValue();
  }
  return graphData;
}
