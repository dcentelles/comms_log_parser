#ifndef CUSTOMAXISTICKER_H
#define CUSTOMAXISTICKER_H

#include <memory>
#include <qcustomplot.h>

class QCPAxisTickerFixedCustom : public QCPAxisTickerFixed {

private:
  QString getTickLabel(double tick, const QLocale &locale, QChar formatChar,
                       int precision);
};

#endif
