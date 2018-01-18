#include <comms_log_parser/mainwindow.h>
#include <ui_mainwindow.h>
#include <QFileDialog>
#include <QDebug>
#include <comms_log_parser/dataplotwindow.h>
#include <comms_log_parser/normalplot.h>
#include <comms_log_parser/jitterplotwindow.h>
#include <comms_log_parser/end2endplotwindow.h>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  init();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::updateRegex()
{
  logTimeFormat = "\\[(\\d+-\\d+-\\d+ \\d+:\\d+:\\d+\\.\\d+)\\]";

  auto txPattern = ui->txRegexLineEdit->text();

  dlTxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    //"TX 2->1:.*\\(Seq: (\\d+)\\) \\(FS: (\\d+)\\)"
    //"TX: transmitting frame... \\(Seq: (\\d+)\\) \\(FS: (\\d+)\\)"
                                                txPattern
      ));

  auto rxPattern = ui->rxRegexLineEdit->text();

  dlRxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    //"RX 1<-2:.*received frame without errors \\(Seq: (\\d+)\\) \\(FS: (\\d+)\\)"
    //"RX: received frame without errors \\(Seq: (\\d+)\\) \\(FS: (\\d+)\\)"
                                                rxPattern
      ));

  auto errPattern = ui->errRegexLineEdit->text();

  dlErrPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    //"RX 1<-2: received frame with errors. Frame will be discarted \\(Seq: (\\d+)\\) \\(FS: (\\d+)\\)"
    //"RX: received frame with errors. Frame will be discarded \\(Seq: (\\d+)\\) \\(FS: (\\d+)\\)"
                                                 errPattern
      ));
}

void MainWindow::init()
{
  //appDefaultPath = "/home/centelld/programming/catkin_ws/src/build-dccomms_ros-Desktop-Debug/devel/lib/dccomms_ros";
  appDefaultPath = ui->mainFolderLineEdit->text();

  updateRegex();

  _plotOver = true;
  ui->plotOverCheckBox->setChecked (_plotOver);
  _lastPlotWindow = NULL;

}


void MainWindow::parseTimes(QList<DataRegisterPtr> & coll,
                            const QString & fileName,
                            const QRegularExpression & reg,
                            QComboBox * t0,
                            QComboBox * t1
                            )
{
  QFile data(fileName);
  if(data.open(QFile::ReadOnly)){
      QTextStream stream(&data);
      QString line;
      coll.clear ();
      while(stream.readLineInto(&line))
        {
          auto match = reg.match (line);
          if(match.hasMatch ())
          {
            auto moment = match.captured(1);
            auto nseq = match.captured(2).toInt();
            auto size = match.captured(3).toInt();

            auto dataRegister = DataRegister::Build(size, moment);
            dataRegister->SetNseq(nseq);
            coll.append(dataRegister);
          }
        }
      if(t0 != NULL && t1 != NULL)
        {
          t0->clear ();
          t1->clear ();

          for(auto it = coll.begin ();
              it != coll.end();
              it++)
            {

              t0->addItem ((*it)->GetDateTimeAsString ());
              t1->addItem ((*it)->GetDateTimeAsString ());
            }
        }
    }
  data.close();

  if(coll.count() > 1 && t0 != NULL && t1 != NULL)
  {
      //auto idx0 = t0->findData(coll[0]->GetDateTimeAsString ());
      //auto idx1 = t1->findData (coll[coll.count()-1]->GetDateTimeAsString ());

      t0->setCurrentIndex (0);
      t1->setCurrentIndex (coll.count()-1);
  }
}


void MainWindow::updateLineEditText(QLineEdit * le, const QString & txt)
{
    le->clear();
    le->insert (txt);
}

void MainWindow::on_dl_computeButton_clicked()
{
    QList<DataRegisterPtr> txDataListFiltered,
                           rxDataListFiltered;

    computeData(
                ui->dl_txT0,
                ui->dl_txT1,
                ui->dl_rxT0,
                ui->dl_rxT1,

                ui->dl_sendLineEdit,

                ui->dl_txGapLineEdit,
                ui->dl_txGapSdLineEdit,

                ui->dl_recvLineEdit,
                ui->dl_failsLineEdit,
                ui->dl_errLineEdit,
                ui->dl_lostLineEdit,

                ui->dl_rxGapLineEdit,
                ui->dl_rxGapSdLineEdit,

                ui->dl_rxDataRate,
                ui->dl_txDataRate,

                dlTxDataList,
                dlRxDataList,

                dlErrDataList,

                txDataListFiltered,
                rxDataListFiltered
                );

}

void MainWindow::on_dl_txBrowseButton_clicked()
{
  dlTxFileName = QFileDialog::getOpenFileName (this,
          tr("Open Dl Tx File"),
          appDefaultPath,
          tr("All files (*)"));
  ui->dl_txFileLineEdit->clear();
  ui->dl_txFileLineEdit->insert(dlTxFileName);
}

void MainWindow::on_dl_rxBrowseButton_clicked()
{
  dlRxFileName = QFileDialog::getOpenFileName (this,
          tr("Open Dl Rx File"),
          appDefaultPath,
          tr("All files (*)"));
  ui->dl_rxFileLineEdit->clear();
  ui->dl_rxFileLineEdit->insert(dlRxFileName);
}

void MainWindow::on_dl_parseTimesButton_clicked()
{
  parseTimes (dlTxDataList,
              dlTxFileName,
              dlTxPattern,
              ui->dl_txT0ComboBox,
              ui->dl_txT1ComboBox);

  parseTimes (dlRxDataList,
              dlRxFileName,
              dlRxPattern,
              ui->dl_rxT0ComboBox,
              ui->dl_rxT1ComboBox);

  parseTimes (dlErrDataList,
              dlRxFileName,
              dlErrPattern,
              NULL,
              NULL);


}


void MainWindow::computeData(QLineEdit * txT0,
                             QLineEdit * txT1,
                             QLineEdit * rxT0,
                             QLineEdit * rxT1,

                             QLineEdit * sendLineEdit,

                             QLineEdit * txGapLineEdit,
                             QLineEdit * txGapSdLineEdit,

                             QLineEdit * recvLineEdit,
                             QLineEdit * failsLineEdit,
                             QLineEdit * errLineEdit,
                             QLineEdit * lostLineEdit,

                             QLineEdit * rxGapLineEdit,
                             QLineEdit * rxGapSdLineEdit,

                             QLineEdit * rxDataRateLineEdit,
                             QLineEdit * txDataRateLineEdit,

                             QList<DataRegisterPtr> & txDataList,
                             QList<DataRegisterPtr> & rxDataList,

                             QList<DataRegisterPtr> & errDataList,

                             QList<DataRegisterPtr> & txDataListFiltered,
                             QList<DataRegisterPtr> & rxDataListFiltered)

{
  _t0 = QDateTime::fromString (txT0->text(),
                                  DataRegister::timeFormat);
  _t1 = QDateTime::fromString (rxT1->text(),
                                  DataRegister::timeFormat);


  txDataListFiltered = DataRegister::GetInterval (txDataList, _t0, _t1);

  updateLineEditText(sendLineEdit, QString::number(txDataListFiltered.count ()));

  rxDataListFiltered = DataRegister::GetInterval (rxDataList, _t0, _t1);

  updateLineEditText(recvLineEdit, QString::number(rxDataListFiltered.count ()));

  //Compute Links
  DataRegister::ComputeLinks (
              txDataListFiltered,
              rxDataListFiltered
              );



  DataRegister::GetGapData (txDataListFiltered,
                            txGap, txGapSd);

  updateLineEditText(txGapLineEdit,QString::number(txGap));
  updateLineEditText(txGapSdLineEdit, QString::number(txGapSd));

  totalFallos = txDataListFiltered.count() - rxDataListFiltered.count();

  updateLineEditText(failsLineEdit, QString::number(totalFallos));

  errors = DataRegister::GetInterval (errDataList, _t0, _t1);

  updateLineEditText(errLineEdit, QString::number(errors.count()));
  updateLineEditText(lostLineEdit, QString::number(totalFallos - errors.count()));


  DataRegister::GetRxGapAndComputeJitter (rxDataListFiltered,
                            rxGap, rxGapSd);

  updateLineEditText(rxGapLineEdit, QString::number(rxGap));
  updateLineEditText(rxGapSdLineEdit, QString::number(rxGapSd));

  //DATA RATE

  DataRegister::GetDataRate(txDataListFiltered, txDataRate);
  DataRegister::GetDataRate(rxDataListFiltered, rxDataRate);

  updateLineEditText(rxDataRateLineEdit, QString::number(rxDataRate));
  updateLineEditText(txDataRateLineEdit, QString::number(txDataRate));


  //PLOT gaussians
  //RX GAP
  NormalPlot * gapPlot = new NormalPlot();
  gapPlot->show();

  gapPlot->Plot("RX Time GAP", rxGap, rxGapSd, 2000, 1, "GAP");

  //TX GAP
  NormalPlot * txGapPlot = new NormalPlot();
  txGapPlot->show();

  txGapPlot->Plot("TX Time GAP", txGap, txGapSd, 2000, 1, "GAP");

  //PDU Size
  DataRegister::GetPDUSize(txDataListFiltered, pduSize, pduSizeSd);

  updateLineEditText (ui->dl_packetSizeLineEdit, QString::number(pduSize));
  updateLineEditText (ui->dl_packetSizeSdLineEdit, QString::number(pduSizeSd));


  //Transmission time
  DataRegister::ComputeTransmissionTime (
              rxDataListFiltered,
              btt,
              bttSd);

  updateLineEditText(ui->dl_transmissionTime, QString::number(btt));
  updateLineEditText(ui->dl_transmissionTimeSD, QString::number(bttSd));

  //PLOT gaussian
  //TT
  NormalPlot * ttPlot = new NormalPlot();
  ttPlot->show();

  ttPlot->Plot("Transmission Time", btt, bttSd, 100, 0.001, "ms/byte");

  //Plot Jitter
  JitterPlotWindow * jitterPlot;

  jitterPlot = new JitterPlotWindow();
  jitterPlot->show();
  jitterPlot->Plot(rxDataListFiltered, "Jitter", _t0, _t1);

  //Plot end 2 end delay per byte
  End2EndPlotWindow * e2ePlot;

  e2ePlot = new End2EndPlotWindow();
  e2ePlot->show();
  e2ePlot->Plot(rxDataListFiltered, "End-End delay per byte", _t0, _t1);
}

void MainWindow::on_dl_txT0ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->dl_txT0->clear();
    ui->dl_txT0->insert(arg1);
}

void MainWindow::on_dl_txT1ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->dl_txT1->clear();
    ui->dl_txT1->insert(arg1);
}

void MainWindow::on_dl_rxT0ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->dl_rxT0->clear();
    ui->dl_rxT0->insert(arg1);
}

void MainWindow::on_dl_rxT1ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->dl_rxT1->clear();
    ui->dl_rxT1->insert(arg1);
}

void MainWindow::on_setIntervalButton_clicked()
{
   auto t0 = ui->t0LineEdit->text();
   auto t1 = ui->t1LineEdit->text();

   ui->dl_txT0->clear();
   ui->dl_txT0->insert(t0);

   ui->dl_txT1->clear();
   ui->dl_txT1->insert(t1);

   ui->dl_rxT0->clear();
   ui->dl_rxT0->insert(t0);

   ui->dl_rxT1->clear();
   ui->dl_rxT1->insert(t1);
}

void MainWindow::on_dl_plotButton_clicked()
{
    auto txTitle = ui->txTitleLineEdit->text();
    auto rxTitle = ui->rxTitleLineEdit->text();
    auto erTitle = ui->erTitleLineEdit->text();

    if(_plotOver && _lastPlotWindow != NULL)
    {
        _lastPlotWindow->PlotOver
                (dlTxDataList,
                 dlRxDataList,
                 dlErrDataList,
                 _t0,
                 _t1,
                 txTitle,
                 rxTitle,
                 erTitle
                );
    }
    else
    {
        DataPlotWindow * dwRx;

        dwRx = new DataPlotWindow();

        dwRx->show();
        /*
        auto rxt0 = QDateTime::fromString (ui->dl_rxT0->text(),
                                        DataRegister::timeFormat);
        auto rxt1 = QDateTime::fromString (ui->dl_rxT1->text(),
                                        DataRegister::timeFormat);
        auto txt0 = QDateTime::fromString (ui->dl_txT0->text(),
                                        DataRegister::timeFormat);
        auto txt1 = QDateTime::fromString (ui->dl_txT1->text(),
                                        DataRegister::timeFormat);

        //Escogemos el intervalo mayor que engloba a todas las muestras de interés:
        QDateTime t0, t1;
        t0 = txt0 <= rxt0 ? txt0 : rxt0;
        t1 = rxt1 >= txt1 ? rxt1 : txt1;
        */

        /*
         * Pasamos todas las muestras del log, pero con el intervalo obtenido para visualizar
         * ese intervalo primero. El usuario podrá interactuar en la línea de tiempo del plot
         * para ver otras muestras que esten fuera del intervalo
         */

        dwRx->Plot(dlTxDataList,
                   dlRxDataList,
                   dlErrDataList,
                   _t0,
                   _t1,
                   txTitle,
                   rxTitle,
                   erTitle
                    );
        _lastPlotWindow = dwRx;
    }
}


void MainWindow::on_plotOverCheckBox_clicked(bool checked)
{
    if(checked)
    {
       _plotOver = true;
    }
    else
    {
      _plotOver = false;
    }
}

void MainWindow::on_pushButton_clicked()
{
  updateRegex();
}

void MainWindow::on_mainFolderLineEdit_textChanged(const QString &arg1)
{
  appDefaultPath = arg1;
}
