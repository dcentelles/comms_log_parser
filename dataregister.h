#ifndef PAQUETTRANSMISSION_H
#define PAQUETTRANSMISSION_H

#include <QDateTime>
#include <QSharedPointer>
#include <QList>

class DataRegister;

typedef QSharedPointer<DataRegister> DataRegisterPtr;

class DataRegister
{

public:
  static DataRegisterPtr Build(int size, const QString & time)
  {
    return DataRegisterPtr(new DataRegister(size, time));
  }
  static DataRegisterPtr Build(int size, const QDateTime & time)
  {
    return DataRegisterPtr(new DataRegister(size, time));
  }
  static DataRegisterPtr Build()
  {
    return DataRegisterPtr(new DataRegister());
  }

  static QList<DataRegisterPtr> GetInterval(
      QList<DataRegisterPtr> data,
      QDateTime t0,
      QDateTime t1);

  static const QString timeFormat;
  DataRegister();
  DataRegister(int size, const QString & time);
  DataRegister(int size, const QDateTime &time);

  void SetDataSize(int);
  void SetDateTimeAsString(const QString &);
  void SetDateTime(const QDateTime &);

  int GetDataSize();
  QString GetDateTimeAsString();
  QDateTime GetDateTime();

  QString ToString();

private:
  void init();
  QDateTime moment;
  int dataSize;

};



#endif // PAQUETTRANSMISSION_H
