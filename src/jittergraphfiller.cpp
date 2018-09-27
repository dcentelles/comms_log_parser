#include <comms_log_parser/jittergraphfiller.h>
#include <ui_jitterplotwindow.h>

JitterGraphFiller::JitterGraphFiller() {}

QVector<QCPGraphData>
JitterGraphFiller::fillGraphData(const QList<DataRegisterPtr> &regs) {

  QVector<QCPGraphData> graphData;
  QCPGraphData gd;

  for (auto it = regs.begin(); it != regs.end(); it++) {
    auto dr = *it;
    if (dr->JitterValid()) {
      auto secSinceEpoch = dr->GetSecs();
      auto secs = secSinceEpoch; // - secsBegin;
      gd.key = secs;
      gd.value = dr->GetJitter();
      graphData.append(gd);
    }
  }
  return graphData;
}
