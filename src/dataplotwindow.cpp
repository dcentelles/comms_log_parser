#include <comms_log_parser/common.h>
#include <comms_log_parser/dataplotwindow.h>

DataPlotWindow::DataPlotWindow(QWidget *parent) : DateTimePlotWindow(parent) {}

DataPlotWindow::~DataPlotWindow() {}

void DataPlotWindow::setDRsToTimeGraph(QCPGraph *graph,
                                       QList<DataRegisterPtr> drs,
                                       bool yTicksTextLabels,
                                       double newYValue) {
  graph->setScatterStyle(QCPScatterStyle::ssDisc);
  graph->setLineStyle(QCPGraph::lsNone);

  QVector<QCPGraphData> dRsGraphData(drs.count());

  if (yTicksTextLabels) {
    for (int i = 0; i < dRsGraphData.count(); i++) {
      auto dr = drs[i];
      auto date = dr->GetSecs();
      // auto secSinceEpoch = (date.toMSecsSinceEpoch () - msT0) / 1000.;
      auto secSinceEpoch = date;
      dRsGraphData[i].key = secSinceEpoch;
      dRsGraphData[i].value = newYValue;
    }
  } else {
    for (int i = 0; i < dRsGraphData.count(); i++) {
      auto dr = drs[i];
      auto date = dr->GetSecs();
      // auto secSinceEpoch = (date.toMSecsSinceEpoch () - msT0) / 1000.;
      auto secSinceEpoch = date;
      dRsGraphData[i].key = secSinceEpoch;
      dRsGraphData[i].value = dr->GetDataSize();
    }
  }

  graph->data()->set(dRsGraphData);
}

void DataPlotWindow::DrawDRsLinksToTimeGraph(QCustomPlot *plot,
                                             QList<DataRegisterPtr> pdus,
                                             bool yTicksTextLabels) {
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
                          const QString &rxtitle, const QString &errtitle,
                          bool plotLinks, bool addErr, bool yTicksTextLabels) {
  // INICIALIZAR PLOT

  QCustomPlot *plot = ui->plotWidget;
  plot->setLocale(QLocale(QLocale::Spanish, QLocale::Spain));

  if (txPdus.count() == 0)
    return;

  plotCount = 0;
  auto t0 = tini / 1e9;
  auto t1 = tend / 1e9;

  plot->xAxis->setRange(t0, t1);
  plot->yAxis->setRange(0, 1200);

  plot->setInteraction(QCP::iRangeDrag, true);
  plot->setInteraction(QCP::iRangeZoom, true);
  updateZoomSettingsFromUi();

  // CREAR GRAFICOS Y ASIGNAR DATOS

  QPen pen;
  // create graph and assign data to it:
  plot->addGraph(); // Paquetes enviados
  auto txGraph = plot->graph(0);
  txGraph->setName(txtitle);
  setDRsToTimeGraph(txGraph, txPdus, yTicksTextLabels, EVENT_TX);
  pen.setColor(QColor(0, 0, 255));
  txGraph->setPen(pen);

  plot->addGraph(); // Paquetes recibidos
  auto rxGraph = plot->graph(1);
  rxGraph->setName(rxtitle);
  setDRsToTimeGraph(rxGraph, rxPdus, yTicksTextLabels, EVENT_RX_OK);
  pen.setColor(QColor(0, 255, 0));
  rxGraph->setPen(pen);

  if (addErr) {
    plot->addGraph(); // Paquetes recibidos con error
    auto errGraph = plot->graph(2);
    errGraph->setName(errtitle);
    setDRsToTimeGraph(errGraph, errors, yTicksTextLabels, EVENT_RX_PERR);
    pen.setColor(QColor(255, 0, 0));
    errGraph->setPen(pen);
  }

  if (plotLinks)
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
                              const QString &rxtitle, const QString &errtitle,
                              bool plotLinks, bool addErr,
                              bool yTicksTextLabels) {
  QCustomPlot *plot = ui->plotWidget;

  if (txPdus.count() == 0)
    return;

  plotCount += 1;
  auto t0 = tini / 1e9;
  auto t1 = tend / 1e9;

  plot->xAxis->setRange(t0, t1);
  plot->yAxis->setRange(0, 1200);

  auto styleNum = plotCount;
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

  // CREAR GRAFICOS Y ASIGNAR DATOS

  auto idx = plot->graphCount();
  QPen pen;
  // create graph and assign data to it:
  plot->addGraph(); // Paquetes enviados
  auto txGraph = plot->graph(idx);
  txGraph->setName(txtitle);
  setDRsToTimeGraph(txGraph, txPdus, yTicksTextLabels, EVENT_TX);
  pen.setColor(QColor(0, 0, 255));
  txGraph->setPen(pen);
  txGraph->setScatterStyle(style);

  plot->addGraph(); // Paquetes recibidos
  auto rxGraph = plot->graph(idx + 1);
  rxGraph->setName(rxtitle);
  setDRsToTimeGraph(rxGraph, rxPdus, yTicksTextLabels, EVENT_RX_OK);
  pen.setColor(QColor(0, 255, 0));
  rxGraph->setPen(pen);
  rxGraph->setScatterStyle(style);

  if (addErr) {
    plot->addGraph(); // Paquetes recibidos con error
    auto errGraph = plot->graph(idx + 2);
    errGraph->setName(errtitle);
    setDRsToTimeGraph(errGraph, errors, yTicksTextLabels, EVENT_RX_PERR);
    pen.setColor(QColor(255, 0, 0));
    errGraph->setPen(pen);
    errGraph->setScatterStyle(style);
  }

  if (plotLinks)
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
                          const QString &rxtitle, const QString &errtitle,
                          bool plotLinks, bool yTicksTextLabels) {
  // INICIALIZAR PLOT

  QCustomPlot *plot = ui->plotWidget;
  plot->setLocale(QLocale(QLocale::Spanish, QLocale::Spain));

  if (txPdus.count() == 0)
    return;

  plotCount = 0;
  auto t0 = tini / 1e9;
  auto t1 = tend / 1e9;

  plot->xAxis->setRange(t0, t1);
  plot->yAxis->setRange(0, 1200);

  plot->setInteraction(QCP::iRangeDrag, true);
  plot->setInteraction(QCP::iRangeZoom, true);
  updateZoomSettingsFromUi();

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

  setDRsToTimeGraph(txGraph, txPdus, yTicksTextLabels, EVENT_TX);
  setDRsToTimeGraph(rxGraph, rxPdus, yTicksTextLabels, EVENT_RX_OK);
  setDRsToTimeGraph(propErrGraph, propErrors, yTicksTextLabels, EVENT_RX_PERR);
  setDRsToTimeGraph(colErrGraph, colErrors, yTicksTextLabels, EVENT_RX_COL);
  setDRsToTimeGraph(multErrGraph, multErrors, yTicksTextLabels, EVENT_RX_MULT);

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

  if (plotLinks)
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
                              const QString &rxtitle, const QString &errtitle,
                              bool plotLinks, bool yTicksTextLabels) {
  QCustomPlot *plot = ui->plotWidget;

  if (txPdus.count() == 0)
    return;

  plotCount += 1;
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

  setDRsToTimeGraph(txGraph, txPdus, yTicksTextLabels, EVENT_TX);
  setDRsToTimeGraph(rxGraph, rxPdus, yTicksTextLabels, EVENT_RX_OK);
  setDRsToTimeGraph(propErrGraph, propErrors, yTicksTextLabels, EVENT_RX_PERR);
  setDRsToTimeGraph(colErrGraph, colErrors, yTicksTextLabels, EVENT_RX_COL);
  setDRsToTimeGraph(multErrGraph, multErrors, yTicksTextLabels, EVENT_RX_MULT);

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

  auto styleNum = plotCount;
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

  if (plotLinks)
    DrawDRsLinksToTimeGraph(plot, txPdus);

  // DIBUJAR
  plot->replot();
}
