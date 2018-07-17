#include <comms_log_parser/dataplotwindow.h>

DataPlotWindow::DataPlotWindow(QWidget *parent) : DateTimePlotWindow(parent) {}

DataPlotWindow::~DataPlotWindow() {}

void DataPlotWindow::setDRsToTimeGraph(QCPGraph *graph,
                                       QList<DataRegisterPtr> drs) {
  graph->setScatterStyle(QCPScatterStyle::ssDisc);
  graph->setLineStyle(QCPGraph::lsNone);

  QVector<QCPGraphData> dRsGraphData(drs.count());

  for (int i = 0; i < dRsGraphData.count(); i++) {
    auto dr = drs[i];
    auto date = dr->GetSecs();
    // auto secSinceEpoch = (date.toMSecsSinceEpoch () - msT0) / 1000.;
    auto secSinceEpoch = date;
    dRsGraphData[i].key = secSinceEpoch;
    dRsGraphData[i].value = dr->GetDataSize();
  }

  graph->data()->set(dRsGraphData);
}

void DataPlotWindow::DrawDRsLinksToTimeGraph(QCustomPlot *plot,
                                             QList<DataRegisterPtr> pdus) {
  auto count = pdus.count();
  for (int idx = 0; idx < count; idx++) {
    auto link = pdus.at(idx);
    auto linked = link->GetLinkedRegister();

    if (linked && !linked->disableLink) {

      auto linkTime = link->GetSecs();
      auto linkedTime = linked->GetSecs();

      auto linkSize = link->GetDataSize();
      auto linkedSize = link->GetDataSize();

      QCPItemCurve *curve = new QCPItemCurve(plot);

      curve->start->setCoords(linkTime, linkSize);
      curve->startDir->setCoords(linkTime, linkSize + 20);
      curve->endDir->setCoords(linkedTime, linkedSize + 20);
      curve->end->setCoords(linkedTime, linkedSize);

      curve->setHead(QCPLineEnding::esNone);

      curve->setVisible(true);
    }
  }
}

void DataPlotWindow::Plot(QList<DataRegisterPtr> txPdus,
                          QList<DataRegisterPtr> rxPdus,
                          QList<DataRegisterPtr> errors, uint64_t tini,
                          uint64_t tend, const QString &txtitle,
                          const QString &rxtitle, const QString &errtitle) {
  // INICIALIZAR PLOT

  QCustomPlot *plot = ui->plotWidget;
  plot->setLocale(QLocale(QLocale::Spanish, QLocale::Spain));

  QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
  dateTicker->setDateTimeFormat("HH:mm:ss:zzz");

  plot->xAxis->setTicker(dateTicker);
  plot->xAxis->setTickLabelRotation(45);

  if (txPdus.count() == 0)
    return;

  // set a more compact font size for bottom and left axis tick labels:
  plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
  plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

  auto t0 = tini / 1e9;
  auto t1 = tend / 1e9;

  plot->xAxis->setRange(t0, t1);
  plot->yAxis->setRange(0, 1200);

  // show legend with slightly transparent background brush:
  plot->legend->setVisible(true);
  plot->legend->setBrush(QColor(255, 255, 255, 150));
  plot->legend->setFont(QFont(QFont().family(), 6));

  plot->setInteraction(QCP::iRangeDrag, true);
  plot->setInteraction(QCP::iRangeZoom, true);
  updateZoomSettingsFromUi();

  plot->xAxis->setLabel("Time");
  plot->yAxis->setLabel("PDU size (bytes)");

  // CREAR GRAFICOS Y ASIGNAR DATOS

  // create graph and assign data to it:
  plot->addGraph(); // Paquetes enviados
  plot->addGraph(); // Paquetes recibidos
  plot->addGraph(); // Paquetes recibidos con error

  auto txGraph = plot->graph(0);
  txGraph->setName(txtitle);
  auto rxGraph = plot->graph(1);
  rxGraph->setName(rxtitle);
  auto errGraph = plot->graph(2);
  errGraph->setName(errtitle);

  setDRsToTimeGraph(txGraph, txPdus);
  setDRsToTimeGraph(rxGraph, rxPdus);
  setDRsToTimeGraph(errGraph, errors);

  QPen pen;
  pen.setColor(QColor(0, 0, 255));
  txGraph->setPen(pen);
  pen.setColor(QColor(0, 255, 0));
  rxGraph->setPen(pen);
  pen.setColor(QColor(255, 0, 0));
  errGraph->setPen(pen);

  DrawDRsLinksToTimeGraph(plot, txPdus);

  // DIBUJAR
  plot->replot();

  QSpinBox *sb = ui->tickStepSpinBox;
  sb->setValue(plot->xAxis->ticker()->tickCount());
}

void DataPlotWindow::PlotOver(QList<DataRegisterPtr> txPdus,
                              QList<DataRegisterPtr> rxPdus,
                              QList<DataRegisterPtr> errors, uint64_t tini,
                              uint64_t tend, const QString &txtitle,
                              const QString &rxtitle, const QString &errtitle) {
  QCustomPlot *plot = ui->plotWidget;

  if (txPdus.count() == 0)
    return;

  // set a more compact font size for bottom and left axis tick labels:
  plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
  plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

  auto t0 = tini / 1e9;
  auto t1 = tend / 1e9;

  plot->xAxis->setRange(t0, t1);
  plot->yAxis->setRange(0, 1200);

  // CREAR GRAFICOS Y ASIGNAR DATOS

  auto idx = plot->graphCount();
  // create graph and assign data to it:
  plot->addGraph(); // Paquetes enviados
  plot->addGraph(); // Paquetes recibidos
  plot->addGraph(); // Paquetes recibidos con error

  auto txGraph = plot->graph(idx);
  txGraph->setName(txtitle);
  auto rxGraph = plot->graph(idx + 1);
  rxGraph->setName(rxtitle);
  auto errGraph = plot->graph(idx + 2);
  errGraph->setName(errtitle);

  setDRsToTimeGraph(txGraph, txPdus);
  setDRsToTimeGraph(rxGraph, rxPdus);
  setDRsToTimeGraph(errGraph, errors);

  QPen pen;
  pen.setColor(QColor(0, 0, 255));
  txGraph->setPen(pen);
  pen.setColor(QColor(0, 255, 0));
  rxGraph->setPen(pen);
  pen.setColor(QColor(255, 0, 0));
  errGraph->setPen(pen);

  auto styleNum = idx / 3;
  QCPScatterStyle style;
  switch (styleNum) {
  case 1:
    style = QCPScatterStyle::ssSquare;
    break;
  case 2:
    style = QCPScatterStyle::ssDiamond;
    break;
  case 3:
    style = QCPScatterStyle::ssDisc;
    break;
  case 4:
    style = QCPScatterStyle::ssCrossSquare;
    break;
  case 5:
    style = QCPScatterStyle::ssCrossCircle;
    break;
  }

  txGraph->setScatterStyle(style);
  rxGraph->setScatterStyle(style);
  errGraph->setScatterStyle(style);

  DrawDRsLinksToTimeGraph(plot, txPdus);

  // DIBUJAR
  plot->replot();
}

void DataPlotWindow::Plot(QList<DataRegisterPtr> txPdus,
                          QList<DataRegisterPtr> rxPdus,
                          QList<DataRegisterPtr> propErrors,
                          QList<DataRegisterPtr> colErrors,
                          QList<DataRegisterPtr> multErrors, uint64_t tini,
                          uint64_t tend, const QString &txtitle,
                          const QString &rxtitle, const QString &errtitle) {
  // INICIALIZAR PLOT

  QCustomPlot *plot = ui->plotWidget;
  plot->setLocale(QLocale(QLocale::Spanish, QLocale::Spain));

  QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
  dateTicker->setDateTimeFormat("HH:mm:ss:zzz");

  plot->xAxis->setTicker(dateTicker);
  plot->xAxis->setTickLabelRotation(45);

  if (txPdus.count() == 0)
    return;

  // set a more compact font size for bottom and left axis tick labels:
  plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
  plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

  auto t0 = tini / 1e9;
  auto t1 = tend / 1e9;

  plot->xAxis->setRange(t0, t1);
  plot->yAxis->setRange(0, 1200);

  // show legend with slightly transparent background brush:
  plot->legend->setVisible(true);
  plot->legend->setBrush(QColor(255, 255, 255, 150));
  plot->legend->setFont(QFont(QFont().family(), 6));

  plot->setInteraction(QCP::iRangeDrag, true);
  plot->setInteraction(QCP::iRangeZoom, true);
  updateZoomSettingsFromUi();

  plot->xAxis->setLabel("Time");
  plot->yAxis->setLabel("PDU size (bytes)");

  // CREAR GRAFICOS Y ASIGNAR DATOS

  // create graph and assign data to it:
  plot->addGraph(); // Paquetes enviados
  plot->addGraph(); // Paquetes recibidos
  plot->addGraph(); // Paquetes recibidos con error
  plot->addGraph(); // Paquetes recibidos con error
  plot->addGraph(); // Paquetes recibidos con error

  auto txGraph = plot->graph(0);
  txGraph->setName(txtitle);
  auto rxGraph = plot->graph(1);
  rxGraph->setName(rxtitle);
  auto propErrGraph = plot->graph(2);
  propErrGraph->setName(errtitle + "(PROP)");
  auto colErrGraph = plot->graph(3);
  colErrGraph->setName(errtitle + "(COL)");
  auto multErrGraph = plot->graph(4);
  multErrGraph->setName(errtitle + "(MULT)");

  setDRsToTimeGraph(txGraph, txPdus);
  setDRsToTimeGraph(rxGraph, rxPdus);
  setDRsToTimeGraph(propErrGraph, propErrors);
  setDRsToTimeGraph(colErrGraph, colErrors);
  setDRsToTimeGraph(multErrGraph, multErrors);

  QPen pen;
  pen.setColor(QColor(0, 0, 255));
  txGraph->setPen(pen);
  pen.setColor(QColor(0, 255, 0));
  rxGraph->setPen(pen);
  pen.setColor(QColor(0, 0, 0));
  propErrGraph->setPen(pen);
  pen.setColor(QColor(255, 0, 0));
  colErrGraph->setPen(pen);
  pen.setColor(QColor(255, 153, 51));
  multErrGraph->setPen(pen);

  DrawDRsLinksToTimeGraph(plot, txPdus);

  // DIBUJAR
  plot->replot();

  QSpinBox *sb = ui->tickStepSpinBox;
  sb->setValue(plot->xAxis->ticker()->tickCount());
}

void DataPlotWindow::PlotOver(QList<DataRegisterPtr> txPdus,
                              QList<DataRegisterPtr> rxPdus,
                              QList<DataRegisterPtr> propErrors,
                              QList<DataRegisterPtr> colErrors,
                              QList<DataRegisterPtr> multErrors, uint64_t tini,
                              uint64_t tend, const QString &txtitle,
                              const QString &rxtitle, const QString &errtitle) {
  QCustomPlot *plot = ui->plotWidget;

  if (txPdus.count() == 0)
    return;

  // set a more compact font size for bottom and left axis tick labels:
  plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
  plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

  auto t0 = tini / 1e9;
  auto t1 = tend / 1e9;

  plot->xAxis->setRange(t0, t1);
  plot->yAxis->setRange(0, 1200);

  // CREAR GRAFICOS Y ASIGNAR DATOS

  auto idx = plot->graphCount();

  // create graph and assign data to it:
  plot->addGraph(); // Paquetes enviados
  plot->addGraph(); // Paquetes recibidos
  plot->addGraph(); // Paquetes recibidos con error
  plot->addGraph(); // Paquetes recibidos con error
  plot->addGraph(); // Paquetes recibidos con error

  auto txGraph = plot->graph(idx);
  txGraph->setName(txtitle);
  auto rxGraph = plot->graph(idx + 1);
  rxGraph->setName(rxtitle);
  auto propErrGraph = plot->graph(idx + 2);
  propErrGraph->setName(errtitle + "(PROP)");
  auto colErrGraph = plot->graph(idx + 3);
  colErrGraph->setName(errtitle + "(COL)");
  auto multErrGraph = plot->graph(idx + 4);
  multErrGraph->setName(errtitle + "(MULT)");

  setDRsToTimeGraph(txGraph, txPdus);
  setDRsToTimeGraph(rxGraph, rxPdus);
  setDRsToTimeGraph(propErrGraph, propErrors);
  setDRsToTimeGraph(colErrGraph, colErrors);
  setDRsToTimeGraph(multErrGraph, multErrors);

  QPen pen;
  pen.setColor(QColor(0, 0, 255));
  txGraph->setPen(pen);
  pen.setColor(QColor(0, 255, 0));
  rxGraph->setPen(pen);
  pen.setColor(QColor(0, 0, 0));
  propErrGraph->setPen(pen);
  pen.setColor(QColor(255, 0, 0));
  colErrGraph->setPen(pen);
  pen.setColor(QColor(255, 153, 51));
  multErrGraph->setPen(pen);

  auto styleNum = idx / 5;
  QCPScatterStyle style;
  switch (styleNum) {
  case 1:
    style = QCPScatterStyle::ssSquare;
    break;
  case 2:
    style = QCPScatterStyle::ssDiamond;
    break;
  case 3:
    style = QCPScatterStyle::ssDisc;
    break;
  case 4:
    style = QCPScatterStyle::ssCrossSquare;
    break;
  case 5:
    style = QCPScatterStyle::ssCrossCircle;
    break;
  }

  txGraph->setScatterStyle(style);
  rxGraph->setScatterStyle(style);
  propErrGraph->setScatterStyle(style);
  colErrGraph->setScatterStyle(style);
  multErrGraph->setScatterStyle(style);

  DrawDRsLinksToTimeGraph(plot, txPdus);

  // DIBUJAR
  plot->replot();
}
