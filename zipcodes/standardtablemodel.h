#pragma once

#include <QStandardItemModel>

class StandardTableModel  : public QStandardItemModel
{
	Q_OBJECT

public:
	StandardTableModel(QObject *parent = 0);
	~StandardTableModel();

public:
	QString filename() const { return m_filename; }
	void clear();
	void load(const QString& filename = QString());
	void save(const QString& filename = QString());

private:
	void initialize();

	QString m_filename;
};
