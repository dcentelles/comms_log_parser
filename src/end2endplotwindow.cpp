#include <comms_log_parser/end2endplotwindow.h>
#include <ui_end2endplotwindow.h>

End2EndPlotWindow::End2EndPlotWindow(QWidget *parent) : PlotWindow(parent) {}

QVector<QCPGraphData>
End2EndPlotWindow::fillGraphData(const QDateTime &relativeTo,
                                 const QList<DataRegisterPtr> &regs) {
  QVector<QCPGraphData> graphData(regs.count());

  auto secsBegin = relativeTo.toMSecsSinceEpoch() / 1000.;

  for (int i = 0; i < graphData.count(); i++) {
    auto dr = regs[i];
    auto date = dr->GetDateTime();
    auto secSinceEpoch = (date.toMSecsSinceEpoch()) / 1000.;
    auto secs = secSinceEpoch - secsBegin;
    graphData[i].key = secs;
    graphData[i].value = dr->GetEnd2EndDelay();
  }
  return graphData;
}
