#include <comms_log_parser/jitterplotwindow.h>
#include <ui_jitterplotwindow.h>

JitterPlotWindow::JitterPlotWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::JitterPlotWindow) {
  ui->setupUi(this);
}

JitterPlotWindow::~JitterPlotWindow() { delete ui; }

void JitterPlotWindow::Plot(QList<DataRegisterPtr> rxregs, const QString &title,
                            QDateTime tini, QDateTime tend) {
  _windowTitle = title;
  _rxregs = rxregs;

  setWindowTitle(_windowTitle);

  // INICIALIZAR PLOT
  QCustomPlot *plot = ui->plotWidget;
  plot->setLocale(QLocale(QLocale::Spanish, QLocale::Spain));

  QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
  dateTicker->setDateTimeFormat("HH:mm:ss:zzz");

  plot->xAxis->setTicker(dateTicker);
  plot->xAxis->setTickLabelRotation(45);

  // set a more compact font size for bottom and left axis tick labels:
  plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
  plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

  auto t0 = (tini.toMSecsSinceEpoch()) / 1000.;
  auto t1 = (tend.toMSecsSinceEpoch()) / 1000.;

  // show legend with slightly transparent background brush:
  plot->legend->setVisible(true);
  plot->legend->setBrush(QColor(255, 255, 255, 150));
  plot->legend->setFont(QFont(QFont().family(), 6));

  plot->setInteraction(QCP::iRangeDrag, true);
  plot->setInteraction(QCP::iRangeZoom, true);
  updateZoomSettingsFromUi();

  plot->xAxis->setLabel("Reception time");
  plot->yAxis->setLabel("Jitter");

  plot->xAxis->setRange(t0, t1);

  plot->addGraph();
  QCPGraph *graph = plot->graph(0);
  graph->setName(_windowTitle);
  graph->setScatterStyle(QCPScatterStyle::ssNone);
  graph->setLineStyle(QCPGraph::lsLine);

  QPen pen;
  pen.setColor (QColor(255,0,0));
  graph->setPen(pen);

  QVector<QCPGraphData> graphData(rxregs.count());

  for (int i = 0; i < graphData.count(); i++) {
    auto dr = rxregs[i];
    if (dr->JitterValud()) {
      auto date = dr->GetDateTime();
      auto secSinceEpoch = (date.toMSecsSinceEpoch()) / 1000.;
      graphData[i].key = secSinceEpoch;
      graphData[i].value = dr->GetJitter();
    }
  }
  graph->data()->set(graphData);
  plot->yAxis->rescale();
  //DIBUJAR
  plot->replot();

  QSpinBox * sb = ui->tickStepSpinBox;
  sb->setValue (plot->xAxis->ticker ()->tickCount ());
}

void JitterPlotWindow::on_tickStepSpinBox_valueChanged(int arg1) {
  QCustomPlot *plot = ui->plotWidget;
  auto graph = plot->graph(0);
  plot->xAxis->ticker()->setTickCount(arg1);
  plot->replot();
}

void JitterPlotWindow::on_blockXToggle_toggled(bool checked) {
  updateZoomSettingsFromUi();
}

void JitterPlotWindow::on_blockYToggle_toggled(bool checked) {
  updateZoomSettingsFromUi();
}

void JitterPlotWindow::updateZoomSettingsFromUi() {
  QCustomPlot *plot = ui->plotWidget;
  QCheckBox *blockXT = ui->blockXToggle;
  QCheckBox *blockYT = ui->blockYToggle;
  bool blockX = blockXT->checkState();
  bool blockY = blockYT->checkState();
  if (!blockX) {
    if (!blockY)
      plot->axisRect()->setRangeZoomAxes(plot->xAxis, plot->yAxis);
    else
      plot->axisRect()->setRangeZoomAxes(plot->xAxis, 0);
  } else {
    if (!blockY)
      plot->axisRect()->setRangeZoomAxes(0, plot->yAxis);
    else
      plot->axisRect()->setRangeZoomAxes(0, 0);
  }
}

void JitterPlotWindow::on_saveAsPDFButton_clicked() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save plot"), "", tr("PDF (*.pdf);;All Files (*)"));
  ui->plotWidget->savePdf(fileName);
}
