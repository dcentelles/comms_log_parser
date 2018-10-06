#include <comms_log_parser/end2endgraphfiller.h>

End2EndGraphFiller::End2EndGraphFiller() {}

QVector<QCPGraphData>
End2EndGraphFiller::fillGraphData(const QList<DataRegisterPtr> &regs) {
  QVector<QCPGraphData> graphData;
  QCPGraphData gd;

  for (auto it = regs.begin(); it != regs.end(); it++) {
    auto dr = *it;
    auto secSinceEpoch = dr->GetSecs();
    auto secs = secSinceEpoch; // - secsBegin;
    gd.key = secs;
    gd.value = dr->GetEnd2EndDelay();
    graphData.append(gd);
  }
  return graphData;
}
