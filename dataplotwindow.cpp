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

void DataPlotWindow::setDRsToTimeGraph(double msT0,
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
        auto secSinceEpoch = (date.toMSecsSinceEpoch () - msT0) / 1000.;
        dRsGraphData[i].key = secSinceEpoch;
        dRsGraphData[i].value = dr->GetDataSize ();
    }

    graph->data()->set(dRsGraphData);
}

void DataPlotWindow::Plot(QList<DataRegisterPtr> txPdus,
          QList<DataRegisterPtr> rxPdus,
          QList<DataRegisterPtr> errors,
          QDateTime tini, QDateTime tend)
{
    QCustomPlot * plot = ui->plotWidget;
    plot->setLocale (QLocale(QLocale::Spanish, QLocale::Spain));

    // create graph and assign data to it:
    plot->addGraph ();
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("PDU size (bytes)");

    auto graph = plot->graph (0);

    if(txPdus.count() == 0)
        return;

    //msT0 es el momento en que se transmitió la primera PDU en el Log
    auto msT0 = txPdus[0]->GetDateTime ().toMSecsSinceEpoch ();

    setDRsToTimeGraph (msT0, graph, txPdus);

    QSharedPointer<QCPAxisTickerTime> dateTicker(new QCPAxisTickerTime);
    dateTicker->setTimeFormat ("%m:%s.%z");

    plot->xAxis->setTicker(dateTicker);
    plot->xAxis->setTickLabelRotation (45);

    QSpinBox * sb = ui->tickStepSpinBox;
    sb->setValue (plot->xAxis->ticker ()->tickCount ());

    // set a more compact font size for bottom and left axis tick labels:
    plot->xAxis->setTickLabelFont(QFont(QFont().family(), 6));
    plot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    auto t0 = (tini.toMSecsSinceEpoch () - msT0) / 1000.;
    auto t1 = (tend.toMSecsSinceEpoch () - msT0)/ 1000.;

    plot->xAxis->setRange(t0, t1);
    plot->yAxis->setRange(0, 1200);

    // show legend with slightly transparent background brush:
    plot->legend->setVisible(true);
    plot->legend->setBrush(QColor(255, 255, 255, 150));

    plot->setInteraction (QCP::iRangeDrag, true);
    plot->setInteraction (QCP::iRangeZoom, true);
    updateZoomSettingsFromUi ();
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






