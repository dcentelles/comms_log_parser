#include "dataregister.h"
#include <qmath.h>

const QString DataRegister::timeFormat =  "yyyy-MM-dd HH:mm:ss.zzz";

DataRegister::DataRegister()
{
  init();
}


DataRegister::DataRegister(int size, const QString & time)
{
  init();
  SetDataSize (size);
  SetDateTimeAsString (time);
}

DataRegister::DataRegister(int size, const QDateTime &time)
{
  init();
  SetDataSize (size);
  SetDateTime (time);
}


void DataRegister::init()
{
//  timeFormat = "yyyy-MM-dd HH:mm:ss.zzz";
    _nseq = -1;
}

void DataRegister::SetDataSize(int size)
{
  dataSize = size;
}

void DataRegister::SetDateTimeAsString(const QString & time)
{
  moment = QDateTime::fromString(time,
                    timeFormat
                    );
}
void DataRegister::SetDateTime(const QDateTime &time)
{
  moment = time;
}

QDateTime DataRegister::GetDateTime()
{
  return moment;
}

QString DataRegister::GetDateTimeAsString()
{
  return moment.toString(timeFormat);
}

int DataRegister::GetDataSize ()
{
  return dataSize;
}

void DataRegister::SetNseq(int nseq)
{
    _nseq = nseq;
}

int DataRegister::GetNseq()
{
    return _nseq;
}

void DataRegister::SetRxDateTime (const QDateTime & datetime)
{
    _rxmoment = datetime;
}

QDateTime DataRegister::GetRxDateTime()
{
    return _rxmoment;
}

QString DataRegister::ToString()
{
  return QString("%1 => %2 bytes").arg(
        GetDateTimeAsString (),
        QString::number(GetDataSize())
        );
}
QList<DataRegisterPtr> DataRegister::GetInterval(QList<DataRegisterPtr> data, QDateTime t0, QDateTime t1)
{
  int idx0;
  for(idx0 = 0; idx0 < data.count(); idx0++)
    {
      auto datetime =  data.at(idx0)->GetDateTime ();
       if(t0 < datetime)
       {
         break;
       }
    }

  if(idx0 > 0)
    idx0 -= 1;

  int idx1;
  for(idx1 = idx0; idx1 < data.count(); idx1++)
    {
      auto datetime =  data.at(idx1)->GetDateTime ();
      if(t1 < datetime)
        {
          idx1 -= 1;
          break;
        }
    }
  QList<DataRegisterPtr> result;
  if(idx0 <= idx1)
    result = data.mid(idx0, idx1 - idx0 + 1);

  return result;
}

DataRegisterPtr DataRegister::GetLinkedRegister()
{
    return _link;
}

void DataRegister::SetLinkedRegister(DataRegisterPtr link)
{
    _link = link;
}

void DataRegister::ComputeLinks(QList<DataRegisterPtr> txl,
                                QList<DataRegisterPtr> rxl)
{
    int idxrx, idxtx = 0;
    for(idxrx = 0; idxrx < rxl.count(); idxrx++)
    {
        auto rx = rxl.at(idxrx);
        auto nseq = rx->GetNseq ();
        auto rxTime = rx->GetDateTime();
        bool foundSender = false;

        DataRegisterPtr tx;
        int initx = idxtx;
        while(!foundSender)
        {
            tx = txl.at(idxtx);
            auto txTime = tx->GetDateTime();
            if(txTime < rxTime)
            {
                auto txnseq = tx->GetNseq();
                if(txnseq == nseq)
                {
                    foundSender = true;
                }
                idxtx++;
            }
            else
            {
                break;
            }
        }
        if(foundSender)
        {
            rx->SetLinkedRegister(tx);
            tx->SetLinkedRegister(rx);
        }
        else
        {
            rx->SetLinkedRegister(DataRegisterPtr(0));
            idxtx = initx;
        }
    }
}

void DataRegister::ComputeTransmissionTime(
        QList<DataRegisterPtr> rxl,
        float & btt,
        float & bttSd)
{
    btt = 0;
    bttSd = 0;
    int count = 0;
    for(auto rx : rxl)
    {
        auto tx = rx->GetLinkedRegister ();
        if(tx)
        {
            auto txTime = tx->GetDateTime();
            auto rxTime = rx->GetDateTime();
            auto gap = txTime.msecsTo (rxTime);
            tx->_ptt = gap;
            rx->_ptt = gap;
            auto psize = tx->GetDataSize ();
            float _btt = (float) gap / psize;
            btt += _btt;
            count++;
        }
    }

    btt /= count;
    for(auto rx : rxl)
    {
        auto tx = rx->GetLinkedRegister ();
        if(tx)
        {
            auto gap = rx->_ptt;
            auto psize = tx->GetDataSize ();
            float _btt = (float) gap / psize;
            auto diff = _btt - btt;
            bttSd += diff*diff;
        }
    }
    bttSd = qSqrt(bttSd / count);
}

void DataRegister::GetGapData(QList<DataRegisterPtr> data, float & gap,
                                                   float & gapSd)
{
    gap = 0;
    gapSd = 0;

    if(data.count() > 0)
    {
        auto t0 = data[0]->GetDateTime ();
        for (auto reg : data)
        {
          auto t1 = reg->GetDateTime ();
          gap += t0.msecsTo (t1);
          t0 = t1;
        }
        gap = gap / data.count();

        t0 = data[0]->GetDateTime ();
        for (auto reg : data)
        {
          auto t1 = reg->GetDateTime ();
          auto _gap = t0.msecsTo (t1);
          auto diff = _gap-gap;
          gapSd += diff*diff;
          t0 = t1;
        }
        gapSd = qSqrt (gapSd/data.count());
    }
}

void DataRegister::GetDataRate(QList<DataRegisterPtr> data, float & dataRate)
{
    dataRate = 0;

    if(data.count() > 1)
    {
        auto tini = data[0]->GetDateTime ();
        auto tend = data[data.count()-1]->GetDateTime ();
        auto elapsed = tini.msecsTo (tend);

        for (auto reg : data)
        {
          dataRate += reg->GetDataSize ();
        }

        float elapsedSec = elapsed / 1000.;

        dataRate = dataRate*8 / elapsedSec; //to bps
        dataRate = dataRate / 1000; //to kbps
    }
}

void DataRegister::GetPDUSize(QList<DataRegisterPtr> data, float & pduSize,
                                                        float & pduSizeSd)
{
    pduSize = 0;
    pduSizeSd = 0;

    if(data.count() > 0)
    {
        for (auto reg : data)
        {
            pduSize += reg->GetDataSize ();
        }
        pduSize = pduSize / data.count();

        for (auto reg : data)
        {
          auto _pduSize = reg->GetDataSize();
          auto diff = _pduSize - pduSize;
          pduSizeSd += diff*diff;
        }
        pduSizeSd = qSqrt (pduSizeSd/data.count());
    }
}
