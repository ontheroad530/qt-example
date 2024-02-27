#pragma once

#include <QDialog>

class QDialogButtonBox;
class QGraphicsScene;
class QPoint;
class TextEdit;
class TextItem;

class TextItemDialog  : public QDialog
{
	Q_OBJECT

public:
	TextItemDialog(TextItem* item_ = 0, const QPoint& position_ = QPoint(),
		QGraphicsScene* scene_ = 0, QWidget *parent = 0);
	~TextItemDialog();

	TextItem* textItem() const { return item; }

public slots:
	void accept();

private slots:
	void updateUi();

private:
	void createWidgets();
	void createLayout();
	void createConnections();

	TextEdit* textEdit;
	QDialogButtonBox* buttonBox;

	TextItem* item;
	QPoint position;
	QGraphicsScene* scene;
	QString originalHtml;
};
