#ifndef NORMALPLOT_H
#define NORMALPLOT_H

#include <QMainWindow>
#include <qcustomplot.h>

namespace Ui {
class NormalPlot;
}

class NormalPlot : public QMainWindow
{
    Q_OBJECT

public:
    explicit NormalPlot(QWidget *parent = 0);
    ~NormalPlot();
    void Plot(const QString & title, float mean, float sd, int off, float inc, const QString & xlabel);

private slots:
   void on_tickStepSpinBox_valueChanged(int arg1);

   void on_blockXToggle_toggled(bool checked);

   void on_blockYToggle_toggled(bool checked);

private:
    Ui::NormalPlot *ui;
    void updateZoomSettingsFromUi();
    void plot();

    QString _xlabel, _windowTitle;
    float _mean, _sd, _inc;
    int _off;


};

#endif // NORMALPLOT_H
