#include <comms_log_parser/plotwindow.h>
#include <ui_plotwindow.h>

PlotWindow::PlotWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PlotWindow) {
  ui->setupUi(this);
  connect(ui->plotWidget->xAxis, SIGNAL(rangeChanged(QCPRange)), this,
          SLOT(xAxisChanged(QCPRange)));
  connect(ui->plotWidget->yAxis, SIGNAL(rangeChanged(QCPRange)), this,
          SLOT(yAxisChanged(QCPRange)));
}

PlotWindow::~PlotWindow() { delete ui; }

void PlotWindow::Plot(QList<DataRegisterPtr> rxregs, const QString &title,
                      QDateTime tini, QDateTime tend, const QString &ylabel,
                      const QString &xlabel) {
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

  QDateTime max, min;
  max = tend.addSecs(3600);
  min = tini.addSecs(-3600);
  ui->t0dateTimeEdit->setMaximumDateTime(max);
  ui->t0dateTimeEdit->setMinimumDateTime(min);
  ui->t0dateTimeEdit->setTime(tini.time());
  auto msdiff = tini.time().msecsTo(tend.time());
  QTime tdiff(0, 0);
  tdiff = tdiff.addMSecs(msdiff);
  ui->durationDateTimeEdit->setTime(tdiff);

  // show legend with slightly transparent background brush:
  plot->legend->setVisible(true);
  plot->legend->setBrush(QColor(255, 255, 255, 150));
  plot->legend->setFont(QFont(QFont().family(), 6));

  plot->setInteraction(QCP::iRangeDrag, true);
  plot->setInteraction(QCP::iRangeZoom, true);
  updateZoomSettingsFromUi();

  plot->xAxis->setLabel(xlabel);
  plot->yAxis->setLabel(ylabel);

  plot->xAxis->setRange(t0, t1);

  auto lower = plot->yAxis->range().lower;
  auto upper = plot->yAxis->range().upper;

  ui->yLowLineEdit->setText(QString::number(lower));
  ui->yHighLineEdit->setText(QString::number(upper));

  plot->addGraph();
  QCPGraph *graph = plot->graph(0);
  graph->setName(_windowTitle);
  graph->setScatterStyle(QCPScatterStyle::ssNone);
  graph->setLineStyle(QCPGraph::lsLine);

  QPen pen;
  pen.setColor(QColor(255, 0, 0));
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
  // DIBUJAR
  plot->replot();

  QSpinBox *sb = ui->tickStepSpinBox;
  sb->setValue(plot->xAxis->ticker()->tickCount());
}

void PlotWindow::on_tickStepSpinBox_valueChanged(int arg1) {
  QCustomPlot *plot = ui->plotWidget;
  auto graph = plot->graph(0);
  plot->xAxis->ticker()->setTickCount(arg1);
  plot->replot();
}

void PlotWindow::on_blockXToggle_toggled(bool checked) {
  updateZoomSettingsFromUi();
}

void PlotWindow::on_blockYToggle_toggled(bool checked) {
  updateZoomSettingsFromUi();
}

void PlotWindow::updateZoomSettingsFromUi() {
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

void PlotWindow::on_saveAsPDFButton_clicked() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save plot"), "", tr("PDF (*.pdf);;All Files (*)"));
  ui->plotWidget->savePdf(fileName);
}

void PlotWindow::updateXAxisRangeFromInput() {
  QDateTime t0 = ui->t0dateTimeEdit->dateTime();
  QDateTime duration = ui->durationDateTimeEdit->dateTime();
  QTime aux(0, 0);
  auto ms = aux.msecsTo(duration.time());
  QDateTime t1 = t0.addMSecs(ms);
  auto t0sec = (t0.toMSecsSinceEpoch()) / 1000.;
  auto t1sec = (t1.toMSecsSinceEpoch()) / 1000.;
  ui->plotWidget->xAxis->setRange(t0sec, t1sec);
  ui->plotWidget->replot();
}
void PlotWindow::on_fixXPushButton_clicked() { updateXAxisRangeFromInput(); }

void PlotWindow::on_fixYPushButton_clicked() {

  auto yTop = ui->yHighLineEdit->text().toInt();
  auto yBottom = ui->yLowLineEdit->text().toInt();
  auto plot = ui->plotWidget;
  plot->yAxis->setRange(yTop, yBottom);
  plot->replot();
}

void PlotWindow::xAxisChanged(QCPRange range) {
  QDateTime t0, t1;
  auto t0sec = range.lower;
  auto t1sec = range.upper;
  t0 = t0.fromMSecsSinceEpoch(t0sec * 1000);
  t1 = t1.fromMSecsSinceEpoch(t1sec * 1000);
  auto msdiff = t0.time().msecsTo(t1.time());
  QTime tdiff(0, 0);
  tdiff = tdiff.addMSecs(msdiff);
  ui->t0dateTimeEdit->setTime(t0.time());
  ui->durationDateTimeEdit->setTime(tdiff);
}

void PlotWindow::yAxisChanged(QCPRange range) {
  auto y0 = range.lower;
  auto y1 = range.upper;
  ui->yLowLineEdit->setText(QString::number(y0));
  ui->yHighLineEdit->setText(QString::number(y1));
}
