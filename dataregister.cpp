#include "dataregister.h"


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
         idx0 -= 1;
         break;
       }
    }

  if(idx0 == -1)
    idx0 = 0;

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
    result = data.mid(idx0, idx1 - idx0);

  return result;
}

