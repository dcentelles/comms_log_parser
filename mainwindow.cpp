#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <dataplotwindow.h>

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

void MainWindow::init()
{
  appDefaultPath = "/home/centelld/Escriptori/HIL_test0_20161214_mati/";
  logTimeFormat = "\\[(\\d+-\\d+-\\d+ \\d+:\\d+:\\d+\\.\\d+)\\]";

  dlTxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "TX: transmitting frame... \\(FS: (\\d+)\\)"
      ));

  dlRxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "RX: received frame without errors \\(FS: (\\d+)\\)"
      ));


  dlErrPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "RX: received frame with errors. Frame will be discarded \\(FS: (\\d+)\\)"
      ));

  appTxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "TX: image transmission completed"
      ));

  appRxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "RX: the received trunk is (the last of an image|also the last of an image)"
      ));

  appErrPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "RX: image received with errors... (some packets were lost)"
      ));
}

void MainWindow::on_app_txBrowseButton_clicked()
{
    appTxFileName = QFileDialog::getOpenFileName (this,
            tr("Open App Tx File"),
            appDefaultPath,
            tr("All files (*)"));

    qDebug() << "App Tx File Name: " << appTxFileName;
    ui->app_txFileLineEdit->clear();
    ui->app_txFileLineEdit->insert(appTxFileName);
}

void MainWindow::on_app_rxBrowseButton_clicked()
{
  appRxFileName = QFileDialog::getOpenFileName (this,
          tr("Open App Rx File"),
          appDefaultPath,
          tr("All files (*)"));

  qDebug() << "App Rx File Name: " << appRxFileName;
  ui->app_rxFileLineEdit->clear();
  ui->app_rxFileLineEdit->insert(appRxFileName);
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
            auto size = match.captured(2).toInt();

            auto dataRegister = DataRegister::Build(size, moment);
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
}

void MainWindow::on_app_parseTimesButton_clicked()
{
  parseTimes (appTxDataList,
              appTxFileName,
              appTxPattern,
              ui->app_txT0ComboBox,
              ui->app_txT1ComboBox);

  parseTimes (appRxDataList,
              appRxFileName,
              appRxPattern,
              ui->app_rxT0ComboBox,
              ui->app_rxT1ComboBox);

  parseTimes (appErrDataList,
              appRxFileName,
              appErrPattern,
              NULL,
              NULL);

}

void MainWindow::on_app_computeButton_clicked()
{
    QList<DataRegisterPtr> txDataListFiltered,
                           rxDataListFiltered;
    computeData(
                ui->app_txT0,
                ui->app_txT1,
                ui->app_rxT0,
                ui->app_rxT1,

                ui->app_sendLineEdit,

                ui->app_txGapLineEdit,
                ui->app_txGapSdLineEdit,

                ui->app_recvLineEdit,
                ui->app_failsLineEdit,
                ui->app_errLineEdit,
                ui->app_lostLineEdit,

                ui->app_rxGapLineEdit,
                ui->app_rxGapSdLineEdit,

                ui->app_rxDataRate,
                ui->app_txDataRate,

                appTxDataList,
                appRxDataList,

                appErrDataList,

                txDataListFiltered,
                rxDataListFiltered
                );
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

    float pduSize, pduSizeSd;
    DataRegister::GetPDUSize(txDataListFiltered, pduSize, pduSizeSd);

    ui->dl_packetSizeLineEdit->clear();
    ui->dl_packetSizeLineEdit->insert(QString::number(pduSize));

    ui->dl_packetSizeSdLineEdit->clear();
    ui->dl_packetSizeSdLineEdit->insert(QString::number(pduSizeSd));
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
                             QList<DataRegisterPtr> & rxDataListFiltered
                             )
{
  //TX
  auto txt0 = QDateTime::fromString (txT0->text(),
                                  DataRegister::timeFormat);
  auto txt1 = QDateTime::fromString (txT1->text(),
                                  DataRegister::timeFormat);

  txDataListFiltered = DataRegister::GetInterval (txDataList, txt0, txt1);

  sendLineEdit->clear();
  sendLineEdit->insert(QString::number(txDataListFiltered.count ()));

  float txGap, txGapSd;

  DataRegister::GetGapData (txDataListFiltered,
                            txGap, txGapSd);

  txGapLineEdit->clear();
  txGapLineEdit->insert(QString::number(txGap));

  txGapSdLineEdit->clear();
  txGapSdLineEdit->insert(QString::number(txGapSd));

  //RX
  auto rxt0 = QDateTime::fromString (rxT0->text(),
                                  DataRegister::timeFormat);
  auto rxt1 = QDateTime::fromString (rxT1->text(),
                                  DataRegister::timeFormat);

  rxDataListFiltered = DataRegister::GetInterval (rxDataList, rxt0, rxt1);

  recvLineEdit->clear();
  recvLineEdit->insert(QString::number(rxDataListFiltered.count ()));

  auto totalFallos = txDataListFiltered.count() - rxDataListFiltered.count();

  failsLineEdit->clear();
  failsLineEdit->insert(QString::number(totalFallos));

  auto errors = DataRegister::GetInterval (errDataList, rxt0, rxt1);

  errLineEdit->clear();
  errLineEdit->insert(QString::number(errors.count()));

  lostLineEdit->clear();
  lostLineEdit->insert(QString::number(totalFallos - errors.count()));

  float rxGap, rxGapSd;

  DataRegister::GetGapData (rxDataListFiltered,
                            rxGap, rxGapSd);

  rxGapLineEdit->clear();
  rxGapLineEdit->insert(QString::number(rxGap));

  rxGapSdLineEdit->clear();
  rxGapSdLineEdit->insert(QString::number(rxGapSd));

  //DATA RATE

  float rxDataRate, txDataRate;

  DataRegister::GetDataRate(txDataListFiltered, txDataRate);
  DataRegister::GetDataRate(rxDataListFiltered, rxDataRate);

  rxDataRateLineEdit->clear();
  rxDataRateLineEdit->insert(QString::number(rxDataRate));

  txDataRateLineEdit->clear();
  txDataRateLineEdit->insert(QString::number(txDataRate));

}

void MainWindow::on_app_txT0ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->app_txT0->clear();
    ui->app_txT0->insert(arg1);
}

void MainWindow::on_app_txT1ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->app_txT1->clear();
    ui->app_txT1->insert(arg1);
}

void MainWindow::on_app_rxT0ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->app_rxT0->clear();
    ui->app_rxT0->insert(arg1);
}

void MainWindow::on_app_rxT1ComboBox_currentIndexChanged(const QString &arg1)
{
    ui->app_rxT1->clear();
    ui->app_rxT1->insert(arg1);
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

   ui->app_txT0->clear();
   ui->app_txT0->insert(t0);

   ui->app_txT1->clear();
   ui->app_txT1->insert(t1);

   ui->app_rxT0->clear();
   ui->app_rxT0->insert(t0);

   ui->app_rxT1->clear();
   ui->app_rxT1->insert(t1);

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

    DataPlotWindow * dwRx;

    dwRx = new DataPlotWindow();

    dwRx->show();

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

    /*
     * Pasamos todas las muestras del log, pero con el intervalo obtenido para visualizar
     * ese intervalo primero. El usuario podrá interactuar en la línea de tiempo del plot
     * para ver otras muestras que esten fuera del intervalo
     */

    dwRx->Plot(dlTxDataList,
               dlRxDataList,
               dlErrDataList,
               t0,
               t1
                );

}
