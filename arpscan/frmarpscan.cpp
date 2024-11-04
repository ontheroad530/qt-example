#include "frmarpscan.h"
#include "arpscan.h"

#include <QDebug>
#include <QDateTime>

enum
{
	TABWGT_IP = 0,
	TABWGT_MAC,

	TABWGT_COLUMN
};

FrmArpScan::FrmArpScan(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FrmArpScanClass())
{
    ui->setupUi(this);

	initForm();
	initArpScan();
}

FrmArpScan::~FrmArpScan()
{
    delete ui;
}

void FrmArpScan::on_btnSearch_clicked()
{
	ui->tableWidget->setRowCount(0);

	QString network = ui->lineEditIP->text();

	m_timestamp = QDateTime::currentMSecsSinceEpoch();
	ArpScan::Instance()->scan(network, m_timestamp);
}

void FrmArpScan::onBeScanned(qint64 id, const QString& ip, const QString& mac)
{
	if (m_timestamp == id)
	{
		qDebug() << ip << " -- " << mac;
		int row = ui->tableWidget->rowCount();

		ui->tableWidget->insertRow(row);

		//NO
		ui->tableWidget->setItem(row, TABWGT_IP, new QTableWidgetItem(ip));
		ui->tableWidget->setItem(row, TABWGT_MAC, new QTableWidgetItem(mac));

		//行居中显示
		for (int col = TABWGT_IP; col < TABWGT_COLUMN; ++col)
		{
			QTableWidgetItem* pItem = ui->tableWidget->item(row, col);
			if (pItem)
			{
				pItem->setTextAlignment(Qt::AlignCenter);
			}
		}
	}
}

void FrmArpScan::initForm()
{
	//设备列表
	ui->tableWidget->setColumnCount(TABWGT_COLUMN);

	QStringList headerText;
	headerText << tr("IP Address") << tr("MAC");
	ui->tableWidget->setHorizontalHeaderLabels(headerText);

	ui->tableWidget->horizontalHeader()->setHighlightSections(false);
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	ui->tableWidget->setColumnWidth(TABWGT_IP, 170);
	ui->tableWidget->setColumnWidth(TABWGT_MAC, 170);
}

void FrmArpScan::initArpScan()
{
	connect(ArpScan::Instance(), &ArpScan::sig_beScanned, this, &FrmArpScan::onBeScanned, Qt::UniqueConnection);
}
