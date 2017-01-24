#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

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

  appTxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "TX: image transmission completed"
      ));

  appRxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat,
    "RX: the received trunk is the last of an image"
      ));
}

void MainWindow::on_app_txBrowseButton_clicked()
{
    appTxFileName = QFileDialog::getOpenFileName (this,
            tr("Open App Tx File"),
            appDefaultPath,
            tr("All files (*)"));

    qDebug() << "App Tx File Name: " << appTxFileName;
    ui->app_rxFileLineEdit->clear();
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
}

void MainWindow::on_app_computeButton_clicked()
{
   auto txt0 = QDateTime::fromString (ui->app_txT0ComboBox->currentText(),
                                   DataRegister::timeFormat);
   auto txt1 = QDateTime::fromString (ui->app_txT1ComboBox->currentText(),
                                   DataRegister::timeFormat);

    auto transmittedImages = DataRegister::GetInterval (appTxDataList, txt0, txt1);

    ui->app_sendLineEdit->clear();
    ui->app_sendLineEdit->insert(QString::number(transmittedImages.count ()));

    auto rxt0 = QDateTime::fromString (ui->app_rxT0ComboBox->currentText(),
                                    DataRegister::timeFormat);
    auto rxt1 = QDateTime::fromString (ui->app_rxT1ComboBox->currentText(),
                                    DataRegister::timeFormat);

    auto receivedImages = DataRegister::GetInterval (appRxDataList, rxt0, rxt1);

    ui->app_recvLineEdit->clear();
    ui->app_recvLineEdit->insert(QString::number(receivedImages.count()));

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


}

void MainWindow::on_dl_computeButton_clicked()
{
  auto txt0 = QDateTime::fromString (ui->dl_txT0ComboBox->currentText(),
                                  DataRegister::timeFormat);
  auto txt1 = QDateTime::fromString (ui->dl_txT1ComboBox->currentText(),
                                  DataRegister::timeFormat);

  auto transmittedPackets = DataRegister::GetInterval (dlTxDataList, txt0, txt1);

  ui->dl_sendLineEdit->clear();
  ui->dl_sendLineEdit->insert(QString::number(transmittedPackets.count ()));

  auto rxt0 = QDateTime::fromString (ui->dl_rxT0ComboBox->currentText(),
                                  DataRegister::timeFormat);
  auto rxt1 = QDateTime::fromString (ui->dl_rxT1ComboBox->currentText(),
                                  DataRegister::timeFormat);

  auto receivedPackets = DataRegister::GetInterval (dlRxDataList, rxt0, rxt1);

  ui->dl_recvLineEdit->clear();
  ui->dl_recvLineEdit->insert(QString::number(receivedPackets.count ()));
}
