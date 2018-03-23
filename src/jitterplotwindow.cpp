#include <comms_log_parser/jitterplotwindow.h>
#include <ui_jitterplotwindow.h>

JitterPlotWindow::JitterPlotWindow(QWidget *parent) : PlotWindow(parent) {}

QVector<QCPGraphData>
JitterPlotWindow::fillGraphData(const QDateTime &relativeTo,
                                const QList<DataRegisterPtr> &regs) {

  QVector<QCPGraphData> graphData(regs.count());

  auto secsBegin = relativeTo.toMSecsSinceEpoch() / 1000.;

  for (int i = 0; i < graphData.count(); i++) {
    auto dr = regs[i];
    if (dr->JitterValud()) {
      auto date = dr->GetDateTime();
      auto secSinceEpoch = (date.toMSecsSinceEpoch()) / 1000.;
      auto secs = secSinceEpoch - secsBegin;
      graphData[i].key = secs;
      graphData[i].value = dr->GetJitter();
    }
  }
  return graphData;
}
