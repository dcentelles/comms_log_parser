#include <comms_log_parser/jittergraphfiller.h>
#include <ui_jitterplotwindow.h>

JitterGraphFiller::JitterGraphFiller() {}

QVector<QCPGraphData>
JitterGraphFiller::fillGraphData(const QList<DataRegisterPtr> &regs) {

  QVector<QCPGraphData> graphData(regs.count());

  for (int i = 0; i < graphData.count(); i++) {
    auto dr = regs[i];
    if (dr->JitterValud()) {
      auto date = dr->GetDateTime();
      auto secSinceEpoch = (date.toMSecsSinceEpoch()) / 1000.;
      auto secs = secSinceEpoch;// - secsBegin;
      graphData[i].key = secs;
      graphData[i].value = dr->GetJitter();
    }
  }
  return graphData;
}
