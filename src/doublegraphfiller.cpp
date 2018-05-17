#include <comms_log_parser/doublegraphfiller.h>
#include <ui_end2endplotwindow.h>

DoubleGraphFiller::DoubleGraphFiller() {}

QVector<QCPGraphData>
DoubleGraphFiller::fillGraphData(const QList<DataRegisterPtr> &regs) {
  QVector<QCPGraphData> graphData;
  QCPGraphData gd;

  for (auto it = regs.begin(); it != regs.end(); it++) {
    auto dr = *it;
    auto secSinceEpoch = dr->GetSecs();
    auto secs = secSinceEpoch; // - secsBegin;
    gd.key = secs;
    gd.value = dr->GetDoubleValue();
    graphData.append(gd);
  }
  return graphData;
}
