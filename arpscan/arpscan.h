#pragma once

#include <QString>
#include <QList>
#include <QPair>

#include <QObject>

#include "singleton.h"

class ArpScan : public QObject
{
	Q_OBJECT SINGLETON_DECL(ArpScan)
	
public:
	ArpScan(QObject* parent = nullptr);
	~ArpScan();

	bool scan(const QString network, qint64 id); //: ip/24

Q_SIGNALS:
	void sig_beScanned(qint64 id, const QString& ip, const QString& mac);

private:
	static bool parseIpMask(const QString& network, QString& ip, uint& slash);
	static ulong getNetSegment1stIP(ulong ip, uint slashMask);
	static void sendArpImpl(qint64 id, ulong nIp);
};

