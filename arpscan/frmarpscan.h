#pragma once

#include <QtWidgets/QWidget>
#include "ui_frmarpscan.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FrmArpScanClass; };
QT_END_NAMESPACE

class FrmArpScan : public QWidget
{
    Q_OBJECT

public:
    FrmArpScan(QWidget *parent = nullptr);
    ~FrmArpScan();

private Q_SLOTS:
	void on_btnSearch_clicked();

	void onBeScanned(qint64 id, const QString& ip, const QString& mac);

private:
	void initForm();
	void initArpScan();

private:
    Ui::FrmArpScanClass *ui;

	qint64	m_timestamp = 0;
};
