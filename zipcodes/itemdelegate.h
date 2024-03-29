#pragma once

#include <QStyledItemDelegate>

class ItemDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ItemDelegate(QObject *parent = 0);
	~ItemDelegate();

	void paint(QPainter* painter, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const;
};
