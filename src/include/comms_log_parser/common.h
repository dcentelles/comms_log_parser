#ifndef COMMS_LOG_PARSER_COMMON_H
#define COMMS_LOG_PARSER_COMMON_H

#include <QString>

#define EVENT_TX 20
#define EVENT_RX_OK 30
#define EVENT_RX_PERR 40
#define EVENT_RX_COL 50
#define EVENT_RX_MULT 60

static QString EVENT_STR_TX = "TX";
static QString EVENT_STR_RX_OK = "RX OK";
static QString EVENT_STR_RX_PERR = "RX PROP.\nERROR";
static QString EVENT_STR_RX_COL = "RX COL.\nERROR";
static QString EVENT_STR_RX_MULT = "RX MULT.\nERRORs";
#endif
