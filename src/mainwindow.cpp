#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include <comms_log_parser/dataplotwindow.h>
#include <comms_log_parser/end2endgraphfiller.h>
#include <comms_log_parser/jittergraphfiller.h>
#include <comms_log_parser/mainwindow.h>
#include <comms_log_parser/normalplot.h>
#include <ui_mainwindow.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  init();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::SaveCurrentSettings(const QString &path) {
  QSettings settings(path, QSettings::NativeFormat);
  settings.setValue("tx_transport_default_dir", _txTransportDefaultDir);
  settings.setValue("rx_transport_default_dir", _rxTransportDefaultDir);
  settings.setValue("distance_default_dir", _distanceDefaultDir);
  settings.setValue("tx_title", ui->txTitleLineEdit->text());
  settings.setValue("rx_title", ui->rxTitleLineEdit->text());
  settings.setValue("er_title", ui->erTitleLineEdit->text());
  settings.setValue("tx_pattern", ui->txRegexLineEdit->text());
  settings.setValue("rx_pattern", ui->rxRegexLineEdit->text());
  settings.setValue("er_pattern", ui->errRegexLineEdit->text());
  int seq_size;
  if (ui->uint8_t_radioButton->isChecked())
    seq_size = 8;
  else if (ui->uint16_t_radioButton->isChecked())
    seq_size = 16;
  else if (ui->uint32_t_radioButton->isChecked())
    seq_size = 32;
  settings.setValue("seq_size", seq_size);

  settings.setValue("packet_size_index", ui->packetSizeIndex_lineEdit->text());
  settings.setValue("packet_size_offset", ui->packetSizeOffsetLineEdit->text());
  settings.setValue("seq_index", ui->seqNumberIndex_lineEdit->text());

  settings.setValue("distance_pattern", ui->distanceRegexLineEdit->text());
  settings.setValue("distance_xlabel", ui->distancesXLabel->text());
  settings.setValue("distance_ylabel", ui->distancesYLabel->text());
  settings.setValue("distance_label", ui->distancesLabel->text());
  settings.setValue("distance_index", ui->distanceIndex_lineEdit->text());
  settings.setValue("transport_tag", ui->tagDescriptionLineEdit->text());

  settings.setValue("common_parser_time_pattern",
                    ui->transportTimePattern->text());
  settings.setValue("common_parser_datetime_format",
                    ui->transportDateTimeFormat->text());

  settings.setValue("common_parser_datetime_index",
                    ui->trDateTimeIndex->text());
  settings.setValue("common_parser_decimals_index",
                    ui->trDecimalsIndex->text());

  settings.setValue("common_parser_time_is_relative", TimeIsRelative() ? 1 : 0);
  settings.setValue("common_parser_relative_time_index",
                    ui->trRelativeTimeIndex->text());

  settings.setValue("settings_last_dir", _lastSettingsFileDir);
}
void MainWindow::LoadSettingsFile(const QString &path) {
  QSettings settings(path, QSettings::NativeFormat);
  _txTransportDefaultDir =
      settings.value("tx_transport_default_dir").toString();
  _rxTransportDefaultDir =
      settings.value("rx_transport_default_dir").toString();
  _distanceDefaultDir = settings.value("distance_default_dir").toString();

  QString txTitle = settings.value("tx_title").toString();
  QString rxTitle = settings.value("rx_title").toString();
  QString erTitle = settings.value("er_title").toString();
  ui->txTitleLineEdit->setText(txTitle);
  ui->rxTitleLineEdit->setText(rxTitle);
  ui->erTitleLineEdit->setText(erTitle);

  QString txPattern = settings.value("tx_pattern").toString();
  QString rxPattern = settings.value("rx_pattern").toString();
  QString erPattern = settings.value("er_pattern").toString();
  ui->txRegexLineEdit->setText(txPattern);
  ui->rxRegexLineEdit->setText(rxPattern);
  ui->errRegexLineEdit->setText(erPattern);

  int seq_size = settings.value("seq_size").toInt();
  if (seq_size == 8)
    ui->uint8_t_radioButton->setChecked(true);
  else if (seq_size == 16)
    ui->uint16_t_radioButton->setChecked(true);
  else if (seq_size == 32)
    ui->uint32_t_radioButton->setChecked(true);

  ui->packetSizeIndex_lineEdit->setText(
      settings.value("packet_size_index").toString());
  ui->packetSizeOffsetLineEdit->setText(
      settings.value("packet_size_offset").toString());
  ui->seqNumberIndex_lineEdit->setText(settings.value("seq_index").toString());

  ui->distanceRegexLineEdit->setText(
      settings.value("distance_pattern").toString());
  ui->distancesXLabel->setText(settings.value("distance_xlabel").toString());
  ui->distancesYLabel->setText(settings.value("distance_ylabel").toString());
  ui->distancesLabel->setText(settings.value("distance_label").toString());
  ui->distanceIndex_lineEdit->setText(
      settings.value("distance_index").toString());

  ui->tagDescriptionLineEdit->setText(
      settings.value("transport_tag").toString());

  ui->transportTimePattern->setText(
      settings.value("common_parser_time_pattern").toString());

  ui->transportDateTimeFormat->setText(
      settings.value("common_parser_datetime_format").toString());

  ui->trDateTimeIndex->setText(
      settings.value("common_parser_datetime_index").toString());
  ui->trDecimalsIndex->setText(
      settings.value("common_parser_decimals_index").toString());
  bool relativeTime = settings.value("common_parser_time_is_relative").toBool();
  SetRelativeTime(relativeTime);

  ui->trRelativeTimeIndex->setText(
      settings.value("common_parser_relative_time_index").toString());

  _lastSettingsFileDir = settings.value("settings_last_dir").toString();
}
void MainWindow::loadDefaultSettings() {
  LoadSettingsFile(_defaultSettingsFile);
}

void MainWindow::saveCurrentSettingsAsDefault() {
  SaveCurrentSettings(_defaultSettingsFile);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  saveCurrentSettingsAsDefault();
  event->accept();
}

bool MainWindow::TimeIsRelative() {
  return ui->trRelativeTimeRadioButton->isChecked();
}

void MainWindow::SetRelativeTime(bool v) {
  ui->trRelativeTimeRadioButton->setChecked(v);
  ui->trDateTimeRadioButton->setChecked(!v);
  on_trDateTimeRadioButton_toggled(!v);
}
int MainWindow::GetPktSizeOffset() {
  return ui->packetSizeOffsetLineEdit->text().toInt();
}

int MainWindow::GetDateTimeIndex() {
  return ui->trDateTimeIndex->text().toInt();
}
int MainWindow::GetDecimalsIndex() {
  return ui->trDecimalsIndex->text().toInt();
}
int MainWindow::GetRelativeValueIndex() {
  return ui->trRelativeTimeIndex->text().toInt();
}

int MainWindow::GetMaxPrefixIndex() {
  QRegularExpression prefixExpr;
  prefixExpr.setPattern(ui->transportTimePattern->text());
  return prefixExpr.namedCaptureGroups().size() - 1;
}

int MainWindow::GetDistanceIndex() {
  int maxPrefixIndex = GetMaxPrefixIndex();
  return ui->distanceIndex_lineEdit->text().toInt() + maxPrefixIndex;
}

int MainWindow::GetSeqIndex() {
  int maxPrefixIndex = GetMaxPrefixIndex();
  return ui->seqNumberIndex_lineEdit->text().toInt() + maxPrefixIndex;
}

int MainWindow::GetPktSizeIndex() {
  int maxPrefixIndex = GetMaxPrefixIndex();
  return ui->packetSizeIndex_lineEdit->text().toInt() + maxPrefixIndex;
}

QString MainWindow::GetTransportTag() {
  return ui->tagDescriptionLineEdit->text();
}

bool MainWindow::GetPlotOver() { return ui->plotOverCheckBox->isChecked(); }

void MainWindow::updateDistanceParser() {
  QString logTimeFormat = ui->transportTimePattern->text();
  auto distancePattern = ui->distanceRegexLineEdit->text();
  _dlDistancePattern.setPattern(
      QString("^%1.*%2").arg(logTimeFormat, distancePattern));
}

void MainWindow::updateTransportParser() {
  QString logTimeFormat = ui->transportTimePattern->text();

  auto txPattern = ui->txRegexLineEdit->text();

  dlTxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat, txPattern));

  auto rxPattern = ui->rxRegexLineEdit->text();

  dlRxPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat, rxPattern));

  auto errPattern = ui->errRegexLineEdit->text();

  dlErrPattern.setPattern(QString("^%1.*%2").arg(logTimeFormat, errPattern));

  if (ui->uint8_t_radioButton->isChecked())
    DataRegister::SetSeqType(DataRegister::SeqType::UINT8);
  else if (ui->uint16_t_radioButton->isChecked())
    DataRegister::SetSeqType(DataRegister::SeqType::UINT16);
  else if (ui->uint32_t_radioButton->isChecked())
    DataRegister::SetSeqType(DataRegister::SeqType::UINT32);
}

void MainWindow::init() {
  _defaultSettingsFile = QApplication::applicationDirPath() + "/default.ini";
  loadDefaultSettings();
  updateTransportParser();
  ui->plotOverCheckBox->setChecked(false);
  _lastPlotWindow = NULL;
}

void MainWindow::parseDoubleTrace(QList<DataRegisterPtr> &coll,
                                  const QString &fileName,
                                  const QRegularExpression &reg,
                                  const QString &xlabel, const QString &ylabel,
                                  const QString &seriesLabel, bool plotOver,
                                  int index) {
  QFile data(fileName);
  if (data.open(QFile::ReadOnly)) {
    bool relativeTime = TimeIsRelative();
    DataRegisterPtr dataRegister;
    int dateTimeIndex = GetDateTimeIndex();
    int decimalsIndex = GetDecimalsIndex();
    int relativeValueIndex = GetRelativeValueIndex();
    QString dateTimeFormat = ui->transportDateTimeFormat->text();
    QTextStream stream(&data);
    QString line;
    coll.clear();
    while (stream.readLineInto(&line)) {
      auto match = reg.match(line);
      if (match.hasMatch()) {
        double value = match.captured(index).toDouble();
        if (relativeTime) {
          double relativeValue = match.captured(relativeValueIndex).toDouble();
          dataRegister = DataRegister::Build(0, relativeValue);
        } else {
          QString dateTimeStr = match.captured(dateTimeIndex);
          QString sdecimals = match.captured(decimalsIndex);
          QDateTime dateTime =
              QDateTime::fromString(dateTimeStr, dateTimeFormat);
          if (!DataRegister::epochSet) {
            DataRegister::epoch = dateTime.addDays(-1).addSecs(3600);
            DataRegister::epochSet = true;
          }
          dataRegister = DataRegister::Build(0, dateTime, sdecimals);
        }
        dataRegister->SetDoubleValue(value);
        coll.append(dataRegister);
      }
    }
    std::shared_ptr<DateTimePlotWindow> plot;
    if (_distancePlotList.size() == 0 || !plotOver) {
      plot = std::shared_ptr<DateTimePlotWindow>(new DateTimePlotWindow());
      plot->SetGraphFiller(GraphFillerPtr(new DoubleGraphFiller()));
      _distancePlotList.push_front(plot);
    } else {
      plot = _distancePlotList.front();
    }
    plot->show();
    uint64_t t0, t1;
    if (coll.size() > 0) {
      t0 = coll[0]->GetNanos();
      t1 = coll[coll.size() - 1]->GetNanos();
      plot->Plot(coll, "Distance", t0, t1, ylabel, xlabel, seriesLabel);
    }
  }
}

void MainWindow::parsePacketTrace(QList<DataRegisterPtr> &coll,
                                  const QString &fileName,
                                  const QRegularExpression &reg, QComboBox *t0,
                                  QComboBox *t1) {

  QFile data(fileName);
  if (data.open(QFile::ReadOnly)) {
    bool relativeTime = TimeIsRelative();
    DataRegisterPtr dataRegister;
    int dateTimeIndex = GetDateTimeIndex();
    int decimalsIndex = GetDecimalsIndex();
    int relativeValueIndex = GetRelativeValueIndex();
    QString dateTimeFormat = ui->transportDateTimeFormat->text();
    QTextStream stream(&data);
    QString line;
    coll.clear();
    auto seqNumIndex = GetSeqIndex();
    auto packetSizeIndex = GetPktSizeIndex();
    while (stream.readLineInto(&line)) {
      auto match = reg.match(line);
      if (match.hasMatch()) {
        uint32_t size, nseq = 0;
        if (seqNumIndex != -1)
          nseq = match.captured(seqNumIndex).toInt();
        size = match.captured(packetSizeIndex).toInt();
        if (relativeTime) {
          double relativeValue = match.captured(relativeValueIndex).toDouble();
          dataRegister =
              DataRegister::Build(size + GetPktSizeOffset(), relativeValue);
        } else {
          QString dateTimeStr = match.captured(dateTimeIndex);
          QString sdecimals = match.captured(decimalsIndex);
          QDateTime dateTime =
              QDateTime::fromString(dateTimeStr, dateTimeFormat);
          if (!DataRegister::epochSet) {
            DataRegister::epoch = dateTime.addDays(-1).addSecs(3600);
            DataRegister::epochSet = true;
          }
          dataRegister = DataRegister::Build(size + GetPktSizeOffset(),
                                             dateTime, sdecimals);
        }
        dataRegister->SetNseq(nseq);
        coll.append(dataRegister);
      }
    }
    if (t0 != NULL && t1 != NULL) {
      t0->clear();
      t1->clear();
      dateTimeFormat = dateTimeFormat + ":zzz"; // show millis
      uint32_t idx = 0;
      for (auto it = coll.begin(); it != coll.end(); it++) {
        QString name = (*it)->GetDateTimeAsString(dateTimeFormat) + " (" +
                       QString::number(idx) + ")";
        t0->addItem(name);
        t1->addItem(name);
        idx++;
      }
    }
  }
  data.close();

  if (coll.count() > 1 && t0 != NULL && t1 != NULL) {
    t0->setCurrentIndex(0);
    t1->setCurrentIndex(coll.count() - 1);
  }
}

DataRegisterPtr
MainWindow::GetDataRegisterFromId(const QString &id,
                                  const QList<DataRegisterPtr> &rlist) {
  QRegularExpression reg;
  reg.setPattern("^.* \\((\\d+)\\)");
  auto match = reg.match(id);
  auto gidx = reg.namedCaptureGroups().size() - 1;
  uint32_t idx = match.captured(gidx).toInt();
  return rlist[idx];
}

void MainWindow::updateLineEditText(QLineEdit *le, const QString &txt) {
  le->clear();
  le->insert(txt);
}

void MainWindow::on_dl_computeButton_clicked() {
  QList<DataRegisterPtr> txDataListFiltered, rxDataListFiltered;

  computeData(

      ui->dl_sendLineEdit,

      ui->dl_txGapLineEdit, ui->dl_txGapSdLineEdit,

      ui->dl_recvLineEdit, ui->dl_failsLineEdit, ui->dl_errLineEdit,
      ui->dl_lostLineEdit,

      ui->dl_rxGapLineEdit, ui->dl_rxGapSdLineEdit,

      ui->dl_rxDataRate, ui->dl_txDataRate,

      dlTxDataList, dlRxDataList,

      dlErrDataList,

      txDataListFiltered, rxDataListFiltered);
}

void MainWindow::on_dl_txBrowseButton_clicked() {
  dlTxFileName = QFileDialog::getOpenFileName(
      this, tr("Open Dl Tx File"), _txTransportDefaultDir, tr("All files (*)"));
  if (dlTxFileName != "") {
    QFileInfo file(dlTxFileName);
    _txTransportDefaultDir = file.absolutePath();
    ui->dl_txFileLineEdit->clear();
    ui->dl_txFileLineEdit->insert(dlTxFileName);
  }
}

void MainWindow::on_dl_rxBrowseButton_clicked() {
  dlRxFileName = QFileDialog::getOpenFileName(
      this, tr("Open Dl Rx File"), _rxTransportDefaultDir, tr("All files (*)"));
  if (dlRxFileName != "") {
    QFileInfo file(dlRxFileName);
    _rxTransportDefaultDir = file.absolutePath();
    ui->dl_rxFileLineEdit->clear();
    ui->dl_rxFileLineEdit->insert(dlRxFileName);
  }
}

void MainWindow::on_dl_parseTimesButton_clicked() {
  updateTransportParser();
  parsePacketTrace(dlTxDataList, dlTxFileName, dlTxPattern, ui->dl_txT0ComboBox,
                   ui->dl_txT1ComboBox);

  parsePacketTrace(dlRxDataList, dlRxFileName, dlRxPattern, ui->dl_rxT0ComboBox,
                   ui->dl_rxT1ComboBox);

  parsePacketTrace(dlErrDataList, dlRxFileName, dlErrPattern, NULL, NULL);

  if (dlTxDataList.size() && dlRxDataList.size()) {
    auto t0reg = GetDataRegisterFromId(ui->dl_txT0->text(), dlTxDataList);
    auto tnreg = GetDataRegisterFromId(ui->dl_rxT1->text(), dlRxDataList);

    _t0 = t0reg->GetNanos();
    _t1 = tnreg->GetNanos();
  }
}

void MainWindow::computeData(
    QLineEdit *sendLineEdit,

    QLineEdit *txGapLineEdit, QLineEdit *txGapSdLineEdit,

    QLineEdit *recvLineEdit, QLineEdit *failsLineEdit, QLineEdit *errLineEdit,
    QLineEdit *lostLineEdit,

    QLineEdit *rxGapLineEdit, QLineEdit *rxGapSdLineEdit,

    QLineEdit *rxDataRateLineEdit, QLineEdit *txDataRateLineEdit,

    QList<DataRegisterPtr> &txDataList, QList<DataRegisterPtr> &rxDataList,

    QList<DataRegisterPtr> &errDataList,

    QList<DataRegisterPtr> &txDataListFiltered,
    QList<DataRegisterPtr> &rxDataListFiltered)

{
  QString tagDesc = GetTransportTag();

  txDataListFiltered = DataRegister::GetInterval(txDataList, _t0, _t1);

  updateLineEditText(sendLineEdit, QString::number(txDataListFiltered.count()));

  rxDataListFiltered = DataRegister::GetInterval(rxDataList, _t0, _t1);

  updateLineEditText(recvLineEdit, QString::number(rxDataListFiltered.count()));

  // Compute Links
  DataRegister::ComputeLinks(txDataListFiltered, rxDataListFiltered);

  DataRegister::GetGapData(txDataListFiltered, txGap, txGapSd);

  updateLineEditText(txGapLineEdit, QString::number(txGap));
  updateLineEditText(txGapSdLineEdit, QString::number(txGapSd));

  totalFallos = txDataListFiltered.count() - rxDataListFiltered.count();

  updateLineEditText(failsLineEdit, QString::number(totalFallos));

  errors = DataRegister::GetInterval(errDataList, _t0, _t1);

  updateLineEditText(errLineEdit, QString::number(errors.count()));
  updateLineEditText(lostLineEdit,
                     QString::number(totalFallos - errors.count()));

  DataRegister::GetRxGapAndComputeJitter(rxDataListFiltered, rxGap, rxGapSd);

  updateLineEditText(rxGapLineEdit, QString::number(rxGap));
  updateLineEditText(rxGapSdLineEdit, QString::number(rxGapSd));

  // DATA RATE

  DataRegister::GetDataRate(txDataListFiltered, txDataRate);
  DataRegister::GetDataRate(rxDataListFiltered, rxDataRate);

  updateLineEditText(rxDataRateLineEdit, QString::number(rxDataRate));
  updateLineEditText(txDataRateLineEdit, QString::number(txDataRate));

  // PLOT gaussians
  // RX GAP
  NormalPlot *gapPlot = new NormalPlot();
  gapPlot->show();

  gapPlot->Plot("RX Time GAP", rxGap, rxGapSd, 2000, 1, "GAP");

  // TX GAP
  NormalPlot *txGapPlot = new NormalPlot();
  txGapPlot->show();

  txGapPlot->Plot("TX Time GAP", txGap, txGapSd, 2000, 1, "GAP");

  // PDU Size
  DataRegister::GetPDUSize(txDataListFiltered, pduSize, pduSizeSd);

  updateLineEditText(ui->dl_packetSizeLineEdit, QString::number(pduSize));
  updateLineEditText(ui->dl_packetSizeSdLineEdit, QString::number(pduSizeSd));

  // Plot Jitter
  std::shared_ptr<DateTimePlotWindow> jitterPlot;
  if (jitterPlotList.size() == 0 || !GetPlotOver()) {
    jitterPlot = std::shared_ptr<DateTimePlotWindow>(new DateTimePlotWindow());
    jitterPlot->SetGraphFiller(GraphFillerPtr(new JitterGraphFiller()));
    jitterPlotList.push_front(jitterPlot);
  } else {
    jitterPlot = jitterPlotList.front();
  }

  jitterPlot->show();
  jitterPlot->Plot(rxDataListFiltered, "Jitter", _t0, _t1, "Jitter",
                   "Reception time", tagDesc);
  // Transmission time
  if (GetSeqIndex() != -1 && rxDataListFiltered.size() > 0) {
    DataRegister::ComputeEnd2EndDelay(rxDataListFiltered, btt, bttSd);

    updateLineEditText(ui->dl_transmissionTime, QString::number(btt));
    updateLineEditText(ui->dl_transmissionTimeSD, QString::number(bttSd));

    // PLOT gaussian
    // TT
    NormalPlot *ttPlot = new NormalPlot();
    ttPlot->show();

    ttPlot->Plot("End 2 End Delay", btt, bttSd, 100, 0.001, "ms");

    // Plot end 2 end delay
    std::shared_ptr<DateTimePlotWindow> e2ePlot;
    if (e2ePlotList.size() == 0 || !GetPlotOver()) {
      e2ePlot = std::shared_ptr<DateTimePlotWindow>(new DateTimePlotWindow());
      e2ePlot->SetGraphFiller(GraphFillerPtr(new End2EndGraphFiller()));
      e2ePlotList.push_front(e2ePlot);
    } else {
      e2ePlot = e2ePlotList.front();
    }
    e2ePlot->show();
    e2ePlot->Plot(rxDataListFiltered, "End-End delay", _t0, _t1,
                  "end-end delay (ms)", "Reception time", tagDesc);
  }
}

void MainWindow::on_dl_txT0ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_txT0->clear();
  ui->dl_txT0->insert(arg1);
}

void MainWindow::on_dl_txT1ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_txT1->clear();
  ui->dl_txT1->insert(arg1);
}

void MainWindow::on_dl_rxT0ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_rxT0->clear();
  ui->dl_rxT0->insert(arg1);
}

void MainWindow::on_dl_rxT1ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_rxT1->clear();
  ui->dl_rxT1->insert(arg1);
}

void MainWindow::on_dl_plotButton_clicked() {
  auto txTitle = ui->txTitleLineEdit->text();
  auto rxTitle = ui->rxTitleLineEdit->text();
  auto erTitle = ui->erTitleLineEdit->text();

  if (GetPlotOver() && _lastPlotWindow != NULL) {
    _lastPlotWindow->PlotOver(dlTxDataList, dlRxDataList, dlErrDataList, _t0,
                              _t1, txTitle, rxTitle, erTitle);
  } else {
    DataPlotWindow *dwRx;

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
     * Pasamos todas las muestras del log, pero con el intervalo obtenido para
     * visualizar
     * ese intervalo primero. El usuario podrá interactuar en la línea de tiempo
     * del plot
     * para ver otras muestras que esten fuera del intervalo
     */

    dwRx->Plot(dlTxDataList, dlRxDataList, dlErrDataList, _t0, _t1, txTitle,
               rxTitle, erTitle);
    _lastPlotWindow = dwRx;
  }
}

void MainWindow::on_distancesPathBrowseButton_clicked() {
  dlDistancesFileName = QFileDialog::getOpenFileName(
      this, tr("Open Dl Tx File"), _distanceDefaultDir, tr("All files (*)"));

  if (dlDistancesFileName != "") {
    QFileInfo file(dlDistancesFileName);
    _distanceDefaultDir = file.absolutePath();
    ui->distancesPathLineEdit->clear();
    ui->distancesPathLineEdit->insert(dlDistancesFileName);
  }
}

void MainWindow::on_parseAndPlotDistanceButton_clicked() {
  updateDistanceParser();
  parseDoubleTrace(_distancesDataList, ui->distancesPathLineEdit->text(),
                   _dlDistancePattern, ui->distancesXLabel->text(),
                   ui->distancesYLabel->text(), ui->distancesLabel->text(),
                   ui->distancePlotOverCheck->isChecked(), GetDistanceIndex());
}

void MainWindow::on_trDateTimeRadioButton_toggled(bool checked) {
  ui->transportDateTimeFormat->setEnabled(checked);
  ui->trDecimalsIndex->setEnabled(checked);
  ui->trDateTimeIndex->setEnabled(checked);
  ui->trRelativeTimeIndex->setEnabled(!checked);
}

void MainWindow::on_saveSettingsAsButton_clicked() {
  QString filePath =
      QFileDialog::getSaveFileName(this, tr("Save settings to file"),
                                   _lastSettingsFileDir, tr("All files (*)"));
  if (filePath != "") {
    QFileInfo file(filePath);
    _lastSettingsFileDir = file.absolutePath();
    SaveCurrentSettings(filePath);
  }
}

void MainWindow::on_loadSettingsFromFileButton_clicked() {
  QString filePath =
      QFileDialog::getOpenFileName(this, tr("Open settings file"),
                                   _lastSettingsFileDir, tr("All files (*)"));

  if (filePath != "") {
    QFileInfo file(filePath);
    _lastSettingsFileDir = file.absolutePath();
    LoadSettingsFile(filePath);
  }
}
