#include "normalplot.h"
#include "ui_normalplot.h"

NormalPlot::NormalPlot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NormalPlot)
{
    ui->setupUi(this);
}

NormalPlot::~NormalPlot()
{
    delete ui;
}
void NormalPlot::plot()
{
    setWindowTitle (_windowTitle);
    //INICIALIZAR PLOT
    QCustomPlot * plot = ui->plotWidget;
    plot->setLocale (QLocale(QLocale::Spanish, QLocale::Spain));

    plot->addGraph();
    auto gausGraph = plot->graph (0);

    auto x0 = _mean - _inc * _off;
    auto x1 = _mean + _inc * _off;

    if(x0 < 0) x0 = 0;

    plot->xAxis->setRange(x0, x1);
    plot->yAxis->setRange(0, 1);
    int npoints = (x1-x0)/_inc + 1;
    QVector<double> xd(npoints), yd(npoints);

    double x;
    int i;
    float var = _sd*_sd;
    for(x = x0, i = 0; x <= x1; i++, x+=_inc)
    {
        auto p = ( 1 / ( sqrt(2*var*M_PI) ) ) * exp( -0.5 * pow((x-_mean), 2.0 )/var);
        xd[i] = x;
        yd[i] = p;
    }
    plot->graph(0)->setData(xd, yd);
    plot->setInteraction (QCP::iRangeDrag, true);
    plot->setInteraction (QCP::iRangeZoom, true);

    updateZoomSettingsFromUi ();
    plot->graph(0)->rescaleAxes();
    plot->xAxis->setLabel(_xlabel);

    plot->replot ();

    QSpinBox * sb = ui->tickStepSpinBox;
    sb->setValue (plot->xAxis->ticker ()->tickCount ());
}

void NormalPlot::Plot(const QString & title, float mean, float sd, int off, float inc, const QString & xlabel)
{
    _windowTitle = title;
    _mean = mean;
    _sd = sd;
    _off = off;
    _inc = inc;
    _xlabel = xlabel;
    plot();
}

void NormalPlot::on_tickStepSpinBox_valueChanged(int arg1)
{
    QCustomPlot * plot = ui->plotWidget;
    auto graph = plot->graph (0);
    plot->xAxis->ticker ()->setTickCount (arg1);
    plot->replot ();
}

void NormalPlot::on_blockXToggle_toggled(bool checked)
{
    updateZoomSettingsFromUi ();
}

void NormalPlot::on_blockYToggle_toggled(bool checked)
{
    updateZoomSettingsFromUi ();
}

void NormalPlot::updateZoomSettingsFromUi()
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

void NormalPlot::on_saveAsPDFButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName (this,
                                                     tr("Save plot"), "",
                                                     tr("PDF (*.pdf);;All Files (*)"));
    ui->plotWidget->savePdf (fileName);
}
