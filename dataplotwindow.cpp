#include "dataplotwindow.h"
#include "ui_dataplotwindow.h"

DataPlotWindow::DataPlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataPlotWindow)
{
    ui->setupUi(this);
}

DataPlotWindow::~DataPlotWindow()
{
    delete ui;
}

void DataPlotWindow::setDRsToTimeGraph(
                      QCPGraph * graph,
                      QList<DataRegisterPtr> drs)
{
    graph->setScatterStyle (QCPScatterStyle::ssDisc);
    graph->setLineStyle (QCPGraph::lsNone);

    QVector<QCPGraphData> dRsGraphData(drs.count());

    for(int i = 0; i < dRsGraphData.count(); i++)
    {
        auto dr = drs[i];
        auto date = dr->GetDateTime ();
        //auto secSinceEpoch = (date.toMSecsSinceEpoch () - msT0) / 1000.;
        auto secSinceEpoch = (date.toMSecsSinceEpoch()) / 1000.;
        dRsGraphData[i].key = secSinceEpoch;
        dRsGraphData[i].value = dr->GetDataSize ();
    }

    graph->data()->set(dRsGraphData);
}

void DataPlotWindow::DrawDRsLinksToTimeGraph(
        QCustomPlot * plot,
        QList<DataRegisterPtr> pdus
        )
{
    auto count = pdus.count();
    for(int idx = 0; idx < count ; idx++)
    {
        auto link = pdus.at(idx);
        auto linked = link->GetLinkedRegister();

        if(linked)
        {

            auto linkDate = link->GetDateTime();
            auto linkedDate = linked->GetDateTime();

            auto linkTime = (linkDate.toMSecsSinceEpoch ()) / 1000.;
            auto linkedTime = (linkedDate.toMSecsSinceEpoch ()) / 1000.;

            auto linkSize = link->GetDataSize();
            auto linkedSize = link->GetDataSize();

            QCPItemCurve * curve = new QCPItemCurve(plot);

            curve->start->setCoords(linkTime, linkSize);
            curve->startDir->setCoords(linkTime, linkSize+20);
            curve->endDir->setCoords(linkedTime, linkedSize+20);
            curve->end->setCoords(linkedTime, linkedSize);

            curve->setHead(QCPLineEnding::esNone);

            curve->setVisible (true);
        }
    }
}

void DataPlotWindow::Plot(QList<DataRegisterPtr> txPdus,
          QList<DataRegisterPtr> rxPdus,
          QList<DataRegisterPtr> errors,
          QDateTime tini, QDateTime tend,
          const QString & txtitle,
          const QString & rxtitle,
          const QString & errtitle)
{
    //INICIALIZAR PLOT

    QCustomPlot * plot = ui->plotWidget;
    plot->setLocale (QLocale(QLocale::Spanish, QLocale::Spain));

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat ("HH:mm:ss:zzz");

    plot->xAxis->setTicker(dateTicker);
    plot->xAxis->setTickLabelRotation (45);

    if(txPdus.count() == 0)
        return;

    // set a more compact font size for bottom and left axis tick labels:
    plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
    plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    auto t0 = (tini.toMSecsSinceEpoch ()) / 1000.;
    auto t1 = (tend.toMSecsSinceEpoch ())/ 1000.;

    plot->xAxis->setRange(t0, t1);
    plot->yAxis->setRange(0, 1200);

    // show legend with slightly transparent background brush:
    plot->legend->setVisible(true);
    plot->legend->setBrush(QColor(255, 255, 255, 150));
    plot->legend->setFont (QFont(QFont().family(), 6));


    plot->setInteraction (QCP::iRangeDrag, true);
    plot->setInteraction (QCP::iRangeZoom, true);
    updateZoomSettingsFromUi ();

    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("PDU size (bytes)");

    //CREAR GRAFICOS Y ASIGNAR DATOS

    // create graph and assign data to it:
    plot->addGraph (); //Paquetes enviados
    plot->addGraph (); //Paquetes recibidos
    plot->addGraph (); //Paquetes recibidos con error


    auto txGraph = plot->graph (0);
    txGraph->setName (txtitle);
    auto rxGraph = plot->graph (1);
    rxGraph->setName (rxtitle);
    auto errGraph = plot->graph (2);
    errGraph->setName (errtitle);

    setDRsToTimeGraph (txGraph, txPdus);
    setDRsToTimeGraph (rxGraph, rxPdus);
    setDRsToTimeGraph (errGraph, errors);

    QPen pen;
    pen.setColor (QColor(0,0,255));
    txGraph->setPen(pen);
    pen.setColor (QColor(0,255,0));
    rxGraph->setPen(pen);
    pen.setColor (QColor(255,0,0));
    errGraph->setPen(pen);

    DrawDRsLinksToTimeGraph(
            plot,
            txPdus
            );

    //DIBUJAR
    plot->replot();

    QSpinBox * sb = ui->tickStepSpinBox;
    sb->setValue (plot->xAxis->ticker ()->tickCount ());
}

void DataPlotWindow::PlotOver(QList<DataRegisterPtr> txPdus,
          QList<DataRegisterPtr> rxPdus,
          QList<DataRegisterPtr> errors,
          QDateTime tini, QDateTime tend,
          const QString & txtitle,
          const QString & rxtitle,
          const QString & errtitle)
{
    QCustomPlot * plot = ui->plotWidget;

    if(txPdus.count() == 0)
        return;

    // set a more compact font size for bottom and left axis tick labels:
    plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
    plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    auto t0 = (tini.toMSecsSinceEpoch ()) / 1000.;
    auto t1 = (tend.toMSecsSinceEpoch ())/ 1000.;

    plot->xAxis->setRange(t0, t1);
    plot->yAxis->setRange(0, 1200);

    //CREAR GRAFICOS Y ASIGNAR DATOS

    // create graph and assign data to it:
    plot->addGraph (); //Paquetes enviados
    plot->addGraph (); //Paquetes recibidos
    plot->addGraph (); //Paquetes recibidos con error

    auto txGraph = plot->graph (3);
    txGraph->setName (txtitle);
    auto rxGraph = plot->graph (4);
    rxGraph->setName (rxtitle);
    auto errGraph = plot->graph (5);
    errGraph->setName (errtitle);

    setDRsToTimeGraph (txGraph, txPdus);
    setDRsToTimeGraph (rxGraph, rxPdus);
    setDRsToTimeGraph (errGraph, errors);

    QPen pen;
    pen.setColor (QColor(0,0,255));
    txGraph->setPen(pen);
    pen.setColor (QColor(0,255,0));
    rxGraph->setPen(pen);
    pen.setColor (QColor(255,0,0));
    errGraph->setPen(pen);

    txGraph->setScatterStyle (QCPScatterStyle::ssSquare);
    rxGraph->setScatterStyle (QCPScatterStyle::ssSquare);
    errGraph->setScatterStyle (QCPScatterStyle::ssSquare);

    DrawDRsLinksToTimeGraph(
            plot,
            txPdus
            );

    //DIBUJAR
    plot->replot();
}

void DataPlotWindow::on_tickStepSpinBox_valueChanged(int arg1)
{
    QCustomPlot * plot = ui->plotWidget;
    auto graph = plot->graph (0);
    plot->xAxis->ticker ()->setTickCount (arg1);
    plot->replot ();
}

void DataPlotWindow::on_blockXToggle_toggled(bool checked)
{
    updateZoomSettingsFromUi ();
}

void DataPlotWindow::on_blockYToggle_toggled(bool checked)
{
    updateZoomSettingsFromUi ();
}

void DataPlotWindow::updateZoomSettingsFromUi()
{
    QCustomPlot * plot = ui->plotWidget;
    QCheckBox * blockXT = ui->blockXToggle;
    QCheckBox * blockYT = ui->blockYToggle;
    bool blockX = blockXT->checkState ();
    bool blockY = blockYT->checkState ();
    if(!blockX)
    {
        if(!blockY)
            plot->axisRect ()->setRangeZoomAxes (plot->xAxis, plot->yAxis);
        else
            plot->axisRect ()->setRangeZoomAxes (plot->xAxis, 0);
    }
    else
    {
        if(!blockY)
            plot->axisRect ()->setRangeZoomAxes (0, plot->yAxis);
        else
            plot->axisRect ()->setRangeZoomAxes (0, 0);
    }
}







void DataPlotWindow::on_saveAsPDFButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName (this,
                                                     tr("Save plot"), "",
                                                     tr("PDF (*.pdf);;All Files (*)"));
    ui->plotWidget->savePdf (fileName);
}
