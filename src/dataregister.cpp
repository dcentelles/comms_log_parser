#include <comms_log_parser/dataregister.h>
#include <qmath.h>

DataRegister::SeqType DataRegister::_seqType = UINT8;

DataRegister::DataRegister() { init(); }

DataRegister::DataRegister(int size, double relativeValue) {
  init();
  SetDataSize(size);
  SetRelativeValue(relativeValue);
}

DataRegister::DataRegister(int size, const QDateTime &time,
                           const QString &sdecimals) {
  init();
  SetDataSize(size);
  SetDateTime(time);
  int nd = sdecimals.size();
  int left = nd >= 3 ? 3 : nd;
  QString smillis = sdecimals.left(left);
  while (smillis.size() < 3)
    smillis.append('0');
  uint32_t millis = smillis.toUInt();
  auto dateTime = time.addMSecs(millis);
  SetDateTime(dateTime);
}

void DataRegister::init() {
  //  timeFormat = "yyyy-MM-dd HH:mm:ss.zzz";
  _nseq = -1;
  _jitterValid = false;
}

void DataRegister::SetDataSize(int size) { dataSize = size; }

void DataRegister::SetDateTime(const QDateTime &time) { moment = time; }

QDateTime DataRegister::GetDateTime() { return moment; }

QString DataRegister::GetDateTimeAsString(const QString &format) {
  return moment.toString(format);
}

int DataRegister::GetDataSize() { return dataSize; }

void DataRegister::SetRxDateTime(const QDateTime &datetime) {
  _rxmoment = datetime;
}

QDateTime DataRegister::GetRxDateTime() { return _rxmoment; }

QString DataRegister::ToString() {
  return QString("%1 => %2 bytes")
      .arg(GetDateTimeAsString("yyyy-MM-dd HH:mm:ss.zzz"),
           QString::number(GetDataSize()));
}
QList<DataRegisterPtr> DataRegister::GetInterval(QList<DataRegisterPtr> data,
                                                 QDateTime t0, QDateTime t1) {
  int idx0;
  for (idx0 = 0; idx0 < data.count(); idx0++) {
    auto datetime = data.at(idx0)->GetDateTime();
    if (t0 <= datetime) {
      break;
    }
  }

  int idx1;
  for (idx1 = idx0; idx1 < data.count(); idx1++) {
    auto datetime = data.at(idx1)->GetDateTime();
    if (t1 < datetime) {
      idx1 -= 1;
      break;
    }
  }
  QList<DataRegisterPtr> result;
  if (idx0 <= idx1)
    result = data.mid(idx0, idx1 - idx0 + 1);

  return result;
}

DataRegisterPtr DataRegister::GetLinkedRegister() { return _link; }

void DataRegister::SetLinkedRegister(DataRegisterPtr link) { _link = link; }

void DataRegister::ComputeLinks(QList<DataRegisterPtr> txl,
                                QList<DataRegisterPtr> rxl) {
  if (txl.count() > 0 && rxl.count() > 0) {
    int idxtx;
    auto rxd = rxl.at(0)->GetDateTime();
    for (idxtx = 0; idxtx < txl.count(); idxtx++) {
      auto txd = txl.at(idxtx)->GetDateTime();
      if (txd >= rxd) {
        idxtx--;
        break;
      }
    }

    int idxrx;
    for (idxrx = 0; idxrx < rxl.count(); idxrx++) {
      auto rx = rxl.at(idxrx);
      auto nseq = rx->GetNseq();
      bool foundSender = false;

      DataRegisterPtr tx;
      int initx = idxtx;
      while (!foundSender && idxtx >= 0) {
        tx = txl.at(idxtx);
        auto txnseq = tx->GetNseq();
        if (txnseq == nseq) {
          foundSender = true;
          break;
        }
        idxtx--;
      }
      if (foundSender) {
        rx->SetLinkedRegister(tx);
        tx->SetLinkedRegister(rx);
      } else {
        rx->SetLinkedRegister(DataRegisterPtr(0));
      }

      int nrxidx = idxrx + 1;
      if (nrxidx < rxl.count()) {
        rxd = rxl.at(nrxidx)->GetDateTime();
        idxtx = initx;
        for (; idxtx < txl.count(); idxtx++) {
          auto txd = txl.at(idxtx)->GetDateTime();
          if (txd >= rxd) {
            idxtx--;
            break;
          }
        }
      }
      if (idxtx == txl.count()) {
        idxtx--;
      }
    }
  }
}

void DataRegister::ComputeTimePerByte(QList<DataRegisterPtr> rxl, float &btt,
                                      float &bttSd) {
  btt = 0;
  bttSd = 0;
  int count = 0;
  for (auto rx : rxl) {
    auto tx = rx->GetLinkedRegister();
    if (tx) {
      auto txTime = tx->GetDateTime();
      auto rxTime = rx->GetDateTime();
      auto gap = txTime.msecsTo(rxTime);
      tx->_ptt = gap;
      rx->_ptt = gap;
      auto psize = tx->GetDataSize();
      float _btt = (float)gap / psize;
      btt += _btt;
      rx->_end2EndDelay = (double)gap / psize;
      count++;
    }
  }

  btt /= count;
  for (auto rx : rxl) {
    auto tx = rx->GetLinkedRegister();
    if (tx) {
      auto gap = rx->_ptt;
      auto psize = tx->GetDataSize();
      float _btt = (float)gap / psize;
      auto diff = _btt - btt;
      bttSd += diff * diff;
    }
  }
  bttSd = qSqrt(bttSd / count);
}

void DataRegister::ComputeEnd2EndDelay(QList<DataRegisterPtr> rxl, float &btt,
                                       float &bttSd) {
  btt = 0;
  bttSd = 0;
  int count = 0;
  for (auto rx : rxl) {
    auto tx = rx->GetLinkedRegister();
    if (tx) {
      auto txTime = tx->GetDateTime();
      auto rxTime = rx->GetDateTime();
      auto gap = txTime.msecsTo(rxTime);
      tx->_ptt = gap;
      rx->_ptt = gap;
      double _btt = gap;
      btt += _btt;
      rx->_end2EndDelay = _btt;
      count++;
    }
  }

  btt /= count;
  for (auto rx : rxl) {
    auto tx = rx->GetLinkedRegister();
    if (tx) {
      auto gap = rx->_ptt;
      double _btt = gap;
      auto diff = _btt - btt;
      bttSd += diff * diff;
    }
  }
  bttSd = qSqrt(bttSd / count);
}

void DataRegister::GetGapData(QList<DataRegisterPtr> data, float &gap,
                              float &gapSd) {
  gap = 0;
  gapSd = 0;

  if (data.count() > 0) {
    auto t0 = data[0]->GetDateTime();
    int count = 0;
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto t1 = reg->GetDateTime();
      gap += t0.msecsTo(t1);
      t0 = t1;
      count++;
    }
    gap = gap / count;

    t0 = data[0]->GetDateTime();
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto t1 = reg->GetDateTime();
      auto _gap = t0.msecsTo(t1);
      auto diff = _gap - gap;
      gapSd += diff * diff;
      t0 = t1;
    }
    gapSd = qSqrt(gapSd / count);
  }
}

void DataRegister::GetRxGapAndComputeJitter(QList<DataRegisterPtr> data,
                                            float &gap, float &gapSd) {
  gap = 0;
  gapSd = 0;

  if (data.count() > 0) {
    auto t0 = data[0]->GetDateTime();
    auto seq0 = data[0]->GetNseq();

    int count = 0;
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto seq1 = reg->GetNseq();
      auto next2seq0 = (seq0 + 1) % 255;
      auto t1 = reg->GetDateTime();

      auto _gap = t0.msecsTo(t1);
      if (next2seq0 == seq1) {
        gap += _gap;
        count++;
      }

      // jitter graph example in: http://nile.wpi.edu/NS
      auto seqdiff = seq1 - seq0;
      if (seqdiff == 0)
        seqdiff = 1;
      if (seqdiff > 0) {
        reg->_jitter = _gap / seqdiff;
        reg->_jitterValid = true;
      } else
        reg->_jitterValid = false;

      t0 = t1;
      seq0 = seq1;
    }
    gap = gap / count;

    t0 = data[0]->GetDateTime();
    seq0 = data[0]->GetNseq();
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto seq1 = reg->GetNseq();
      auto next2seq0 = (seq0 + 1) % 255;
      auto t1 = reg->GetDateTime();

      if (next2seq0 == seq1) {
        auto _gap = t0.msecsTo(t1);
        auto diff = _gap - gap;
        gapSd += diff * diff;
      }

      t0 = t1;
      seq0 = seq1;
    }
    gapSd = qSqrt(gapSd / count);
  }
}

void DataRegister::GetDataRate(QList<DataRegisterPtr> data, float &dataRate) {
  dataRate = 0;

  if (data.count() > 1) {
    auto tini = data[0]->GetDateTime();
    auto tend = data[data.count() - 1]->GetDateTime();
    auto elapsed = tini.msecsTo(tend);

    for (auto reg : data) {
      dataRate += reg->GetDataSize();
    }

    float elapsedSec = elapsed / 1000.;

    dataRate = dataRate * 8 / elapsedSec; // to bps
    dataRate = dataRate / 1000;           // to kbps
  }
}

void DataRegister::GetPDUSize(QList<DataRegisterPtr> data, float &pduSize,
                              float &pduSizeSd) {
  pduSize = 0;
  pduSizeSd = 0;

  if (data.count() > 0) {
    for (auto reg : data) {
      pduSize += reg->GetDataSize();
    }
    pduSize = pduSize / data.count();

    for (auto reg : data) {
      auto _pduSize = reg->GetDataSize();
      auto diff = _pduSize - pduSize;
      pduSizeSd += diff * diff;
    }
    pduSizeSd = qSqrt(pduSizeSd / data.count());
  }
}
