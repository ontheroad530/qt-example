#include "arpscan.h"

#include <QDebug>

#include <thread>

#include <WinSock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

SINGLETON_IMPL(ArpScan)
ArpScan::ArpScan(QObject* parent)
{

}

ArpScan::~ArpScan()
{

}

bool ArpScan::scan(const QString network, qint64 id)
{
	//����IP/����
	QString ip;
	uint slashMask = 32;
	if (!parseIpMask(network, ip, slashMask))
	{
		return false;
	}

	//��ȡIP��ַ�������ֽ���
	ulong nIp = inet_addr(ip.toUtf8().data());
	if (INADDR_NONE == nIp)
	{
		qCritical() << "error : " << nIp;
		return false;
	}

	if (slashMask < 24)
	{
		//ֻ֧�� >= 24�������߳���̫��
		qWarning() << QString("%1/%2 fixed to %3/24!!!").arg(ip).arg(slashMask).arg(ip);
		slashMask = 24;
	}

	if (32 != slashMask)
	{
		//��ȡ�������׸�IP��ַ
		nIp = getNetSegment1stIP(nIp, slashMask);
	}
	
	//ѭ������
	uint loopCount = 1 << (32 - slashMask);
	for (uint i = 0; i < loopCount; ++i)
	{
		//�˴�ʹ���߳�detach����������
		std::thread thr(sendArpImpl, id, nIp);
		thr.detach();

		ulong localIp = ntohl(nIp);
		nIp = htonl(++localIp);
	}

	return true;
}

bool ArpScan::parseIpMask(const QString& network, QString& ip, uint& slash)
{
	if (network.isEmpty())
	{
		qCritical("network is empty");
		return false;
	}

	//����IP/MASK
	QStringList strs = network.split('/');
	ip = strs.at(0);
	slash = strs.size() > 1 ? strs.at(1).toInt() : 32;

	return true;
}

ulong ArpScan::getNetSegment1stIP(ulong ip, uint slashMask)
{
	IPAddr oldDestIp = ntohl(ip);
	IPAddr newDestIp = 0;
	ulong mask = 1 << (32 - slashMask);
	for (uint i = 0; i < slashMask; i++) {
		newDestIp += oldDestIp & mask;
		mask <<= 1;
	}
	
	return htonl(++newDestIp);
}

void ArpScan::sendArpImpl(qint64 id, ulong nIp)
{
	IPAddr SrcIp = 0;       /* default for src ip */
	ULONG MacAddr[2];       /* for 6-byte hardware addresses */
	ULONG PhysAddrLen = 6;  /* default to length of six bytes */
	
	memset(&MacAddr, 0xff, sizeof(MacAddr));
	PhysAddrLen = 6;

	DWORD dwRetVal = SendARP(nIp, SrcIp, &MacAddr, &PhysAddrLen);

	if (PhysAddrLen) 
	{
		//ip
		in_addr ip_addr;
		ip_addr.s_addr = nIp;
		QString ip = inet_ntoa(ip_addr);

		//mac
		BYTE* pPhysAddr = (BYTE*)&MacAddr;
		QString mac = QString("%1:%2:%3:%4:%5:%6")
			.arg(pPhysAddr[0], 2, 16, QLatin1Char('0'))
			.arg(pPhysAddr[1], 2, 16, QLatin1Char('0'))
			.arg(pPhysAddr[2], 2, 16, QLatin1Char('0'))
			.arg(pPhysAddr[3], 2, 16, QLatin1Char('0'))
			.arg(pPhysAddr[4], 2, 16, QLatin1Char('0'))
			.arg(pPhysAddr[5], 2, 16, QLatin1Char('0'));

		emit ArpScan::Instance()->sig_beScanned(id, ip, mac);
	}
}
