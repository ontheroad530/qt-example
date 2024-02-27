#include "textitemdialog.h"
#include "../aqp/alt_key.hpp"
#include "swatch.hpp"
#include "textedit.hpp"
#include "textitem.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QGraphicsScene>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

TextItemDialog::TextItemDialog(TextItem* item_, const QPoint& position_,
	QGraphicsScene* scene_, QWidget *parent)
	: QDialog(parent), item(item_), position(position_), scene(scene_)
{
	createWidgets();
	createLayout();
	createConnections();

	AQP::accelerateWidget(this);
	setWindowTitle(tr("%1 - %2 Text Item")
		.arg(QApplication::applicationName())
		.arg(item ? tr("Edit") : tr("Add")));
	updateUi();
}

TextItemDialog::~TextItemDialog()
{}

void TextItemDialog::accept()
{
	if (item && !textEdit->document()->isModified()) {
		QDialog::reject();
		return;
	}
	if (!item)
		item = new TextItem(position, scene);
	item->setHtml(textEdit->toHtml());
	item->update();
	QDialog::accept();
}

void TextItemDialog::updateUi()
{
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
		!textEdit->toPlainText().isEmpty());
}

void TextItemDialog::createWidgets()
{
	textEdit = new TextEdit;
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
		QDialogButtonBox::Cancel);
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	buttonBox->button(QDialogButtonBox::Ok)->setIcon(
		style()->standardIcon(QStyle::SP_DialogOkButton));
	buttonBox->button(QDialogButtonBox::Cancel)->setIcon(
		style()->standardIcon(QStyle::SP_DialogCancelButton));

	if (item)
		textEdit->setHtml(item->toHtml());
}

void TextItemDialog::createLayout()
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(textEdit);
	layout->addWidget(buttonBox);
	setLayout(layout);
}

void TextItemDialog::createConnections()
{
	connect(textEdit, SIGNAL(textChanged()), this, SLOT(updateUi()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}
