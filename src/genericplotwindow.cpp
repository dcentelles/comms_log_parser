#include <comms_log_parser/genericplotwindow.h>
#include <comms_log_parser/doublegraphfiller.h>
#include <ui_genericplotwindow.h>

GenericPlotWindow::GenericPlotWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::GenericPlotWindow) {
  ui->setupUi(this);
  connect(ui->plotWidget->xAxis, SIGNAL(rangeChanged(QCPRange)), this,
          SLOT(xAxisChanged(QCPRange)));
  connect(ui->plotWidget->yAxis, SIGNAL(rangeChanged(QCPRange)), this,
          SLOT(yAxisChanged(QCPRange)));

  colors.append(QColor(0, 0, 255));
  colors.append(QColor(0, 255, 0));
  colors.append(QColor(255, 0, 0));
  colors.append(QColor(255, 255, 0));
  colors.append(QColor(255, 0, 255));
  colors.append(QColor(0, 255, 255));
  colors.append(QColor(255, 255, 255));
  _gf = GraphFillerPtr(new DoubleGraphFiller());
}

void GenericPlotWindow::SetGraphFiller(GraphFillerPtr gf) { _gf = gf; }
GraphFillerPtr GenericPlotWindow::GetGraphFiller() { return _gf; }

GenericPlotWindow::~GenericPlotWindow() { delete ui; }

void GenericPlotWindow::Plot(QList<DataRegisterPtr> regs, const QString &title,
                              double tini, double tend,
                              const QString &ylabel, const QString &xlabel,
                              const QString &tagDesc) {
  _windowTitle = title;
  _rxregs = regs;

  setWindowTitle(_windowTitle);

  // INICIALIZAR PLOT
  QCustomPlot *plot = ui->plotWidget;
  plot->setLocale(QLocale(QLocale::Spanish, QLocale::Spain));

  QSharedPointer<QCPAxisTickerFixed> ticker(new QCPAxisTickerFixed);

  plot->xAxis->setTicker(ticker);
  //plot->xAxis->setTickLabelRotation(45);

  // set a more compact font size for bottom and left axis tick labels:
  plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
  plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

  plot->xAxis->setRange(tini, tend);

  // show legend with slightly transparent background brush:
  plot->legend->setVisible(true);
  plot->legend->setBrush(QColor(255, 255, 255, 150));
  plot->legend->setFont(QFont(QFont().family(), 6));

  plot->setInteraction(QCP::iRangeDrag, true);
  plot->setInteraction(QCP::iRangeZoom, true);
  updateZoomSettingsFromUi();

  plot->xAxis->setLabel(xlabel);
  plot->yAxis->setLabel(ylabel);

  auto lower = plot->yAxis->range().lower;
  auto upper = plot->yAxis->range().upper;

  ui->yLowLineEdit->setText(QString::number(lower));
  ui->yHighLineEdit->setText(QString::number(upper));

  plot->addGraph();
  QCPGraph *graph = plot->graph(plot->graphCount() - 1);
  graph->setName(_windowTitle);
  graph->setScatterStyle(QCPScatterStyle::ssNone);
  graph->setLineStyle(QCPGraph::lsLine);
  graph->setName(tagDesc);

  QPen pen;
  uint32_t colorIndex = (plot->graphCount() - 1) % colors.count();
  pen.setColor(colors[colorIndex]);
  graph->setPen(pen);

  QVector<QCPGraphData> graphData = _gf->fillGraphData(regs);

  graph->data()->set(graphData);
  plot->yAxis->rescale();
  // DIBUJAR
  plot->replot();

  QSpinBox *sb = ui->tickStepSpinBox;
  sb->setValue(plot->xAxis->ticker()->tickCount());
}

void GenericPlotWindow::on_tickStepSpinBox_valueChanged(int arg1) {
  QCustomPlot *plot = ui->plotWidget;
  auto graph = plot->graph(0);
  plot->xAxis->ticker()->setTickCount(arg1);
  plot->replot();
}

void GenericPlotWindow::on_blockXToggle_toggled(bool checked) {
  updateZoomSettingsFromUi();
}

void GenericPlotWindow::on_blockYToggle_toggled(bool checked) {
  updateZoomSettingsFromUi();
}

void GenericPlotWindow::updateZoomSettingsFromUi() {
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

void GenericPlotWindow::on_saveAsPDFButton_clicked() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save plot"), "", tr("PDF (*.pdf);;All Files (*)"));
  ui->plotWidget->savePdf(fileName);
}

void GenericPlotWindow::updateXAxisRangeFromInput() {

  QDateTime t0 = ui->t0dateTimeEdit->dateTime();
  QDateTime duration = ui->durationDateTimeEdit->dateTime();

  auto t0sec = t0.toMSecsSinceEpoch() / 1000.;
  auto durationMs = duration.toMSecsSinceEpoch();
  auto t1sec = t0.addMSecs(durationMs).toMSecsSinceEpoch() / 1000.;
  ui->plotWidget->xAxis->setRange(t0sec, t1sec);
  ui->plotWidget->replot();
}
void GenericPlotWindow::on_fixXPushButton_clicked() {
  updateXAxisRangeFromInput();
}

void GenericPlotWindow::on_fixYPushButton_clicked() {

  auto yTop = ui->yHighLineEdit->text().toDouble();
  auto yBottom = ui->yLowLineEdit->text().toDouble();
  auto plot = ui->plotWidget;
  plot->yAxis->setRange(yTop, yBottom);
  plot->replot();
}

void GenericPlotWindow::xAxisChanged(QCPRange range) {
  QDateTime t0, t1;
  auto t0sec = range.lower;
  auto t1sec = range.upper;
  t0 = t0.fromMSecsSinceEpoch(t0sec * 1000);
  t1 = t1.fromMSecsSinceEpoch(t1sec * 1000);
  QDateTime duration;
  duration = QDateTime::fromMSecsSinceEpoch(t0.msecsTo(t1));
  ui->t0dateTimeEdit->setDateTime(t0);
  ui->durationDateTimeEdit->setDateTime(duration);
}

void GenericPlotWindow::yAxisChanged(QCPRange range) {
  auto y0 = range.lower;
  auto y1 = range.upper;
  ui->yLowLineEdit->setText(QString::number(y0));
  ui->yHighLineEdit->setText(QString::number(y1));
}
