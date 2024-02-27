#include "pagedesigner.h"
#include "graphicsview.h"
#include "brushwidget.h"
#include "penwidget.h"
#include "itemtypes.hpp"
#include "boxitem.h"
#include "textitem.h"
#include "smileyitem.h"
#include "textitemdialog.h"
#include "../aqp/aqp.hpp"
#include "../aqp/alt_key.hpp"
#include "transformwidget.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QImageWriter>
#include <QMenu>
#include <QMenuBar>
#include <QPrintDialog>
#include <QSettings>
#include <QStatusBar>
#include <QSvgGenerator>
#include <QTimer>
#include <QToolBar>
#include <QMimeData>

const int StatusTimeout = AQP::MSecPerSecond * 30;
const int OffsetIncrement = 5;
const qint32 MagicNumber = 0x5A93DE5;
const qint16 VersionNumber = 1;
const QString ShowGrid("ShowGrid");
const QString MostRecentFile("MostRecentFile");
const QString MimeType = "application/vnd.qtrac.pagedesigner";

PageDesigner::PageDesigner(QWidget *parent)
	: QMainWindow(parent), gridGroup(0), addOffset(OffsetIncrement),
	pasteOffset(OffsetIncrement)
{
	printer = new QPrinter(QPrinter::HighResolution);

	createSceneAndView();
	createActions();
	createMenusAndToolBars();
	createDockWidgets();
	createConnections();

	QSettings settings;
	viewShowGridAction->setChecked(
		settings.value(ShowGrid, true).toBool());
	QString filename = settings.value(MostRecentFile).toString();
	if (filename.isEmpty() || filename == tr("Unnamed"))
		QTimer::singleShot(0, this, SLOT(fileNew()));
	else {
		setWindowFilePath(filename);
		QTimer::singleShot(0, this, SLOT(loadFile()));
	}
}

PageDesigner::~PageDesigner()
{
}

QSize PageDesigner::sizeHint() const
{
	QSize size = printer->paperSize(QPrinter::Point).toSize() * 1.2;
	size.rwidth() += brushWidget->sizeHint().width();
	return size.boundedTo(
		QApplication::desktop()->availableGeometry().size());
}

void PageDesigner::setDirty(bool on)
{
	setWindowModified(on);
	updateUi();
}

void PageDesigner::closeEvent(QCloseEvent* event)
{
	if (okToClearData()) {
		QSettings settings;
		settings.setValue(ShowGrid, viewShowGridAction->isChecked());
		settings.setValue(MostRecentFile, windowFilePath());
		event->accept();
	}
	else
		event->ignore();
}

void PageDesigner::fileNew()
{
	if (!okToClearData())
		return;
	clear();
	setWindowFilePath(tr("Unnamed"));
	setDirty(false);
}

void PageDesigner::fileOpen()
{
	if (!okToClearData())
		return;
	const QString& filename = QFileDialog::getOpenFileName(this,
		tr("%1 - Open").arg(QApplication::applicationName()),
		".", tr("Page Designer (*.pd)"));
	if (filename.isEmpty())
		return;
	setWindowFilePath(filename);
	loadFile();
}

bool PageDesigner::fileSave()
{
	const QString filename = windowFilePath();
	if (filename.isEmpty() || filename == tr("Unnamed"))
		return fileSaveAs();
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QDataStream out(&file);
	out << MagicNumber << VersionNumber;
	out.setVersion(QDataStream::Qt_4_5);
	writeItems(out, scene->items());
	file.close();
	setDirty(false);

	return true;
}

bool PageDesigner::fileSaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this,
		tr("%1 - Save As").arg(QApplication::applicationName()),
		".", tr("Page Designer (*.pd)"));
	if (filename.isEmpty())
		return false;
	if (!filename.toLower().endsWith(".pd"))
		filename += ".pd";
	setWindowFilePath(filename);
	return fileSave();
}

void PageDesigner::fileExport()
{
	QString suffixes = AQP::filenameFilter(tr("Bitmap image"),
		QImageWriter::supportedImageFormats());
	suffixes += tr(";;Vector image (*.svg)");
	const QString filename = QFileDialog::getSaveFileName(this,
		tr("%1 - Export").arg(QApplication::applicationName()),
		".", suffixes);
	if (filename.isEmpty())
		return;
	if (filename.toLower().endsWith(".svg"))
		exportSvg(filename);
	else
		exportImage(filename);
}

void PageDesigner::filePrint()
{
	QPrintDialog dialog(printer);
	if (dialog.exec()) {
		{
			QPainter painter(printer);
			paintScene(&painter);
		}
		statusBar()->showMessage(tr("Printed %1")
			.arg(windowFilePath()), StatusTimeout);
	}
}

void PageDesigner::editSelectedItem()
{
	QList<QGraphicsItem*> items = scene->selectedItems();
	if (items.count() != 1)
		return;
#ifdef NO_DYNAMIC_CAST
	if (QObject* item = qObjectFrom(items.at(0))) {
#else
	if (QObject* item = dynamic_cast<QObject*>(items.at(0))) {
#endif
		const QMetaObject* metaObject = item->metaObject();
		metaObject->invokeMethod(item, "edit", Qt::DirectConnection);
	}
}

void PageDesigner::editAddItem()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (!action)
		return;
	QObject* item = 0;
	int type = action->data().toInt();
	if (type == BoxItemType)
		item = new BoxItem(QRect(position(), QSize(90, 30)), scene);
	else if (type == SmileyItemType)
		item = new SmileyItem(position(), scene);
	else if (type == TextItemType) {
		TextItemDialog dialog(0, position(), scene, this);
		if (dialog.exec())
			item = dialog.textItem();
	}
	if (item) {
		connectItem(item);
		setDirty(true);
	}
}

void PageDesigner::editCopy()
{
	QList<QGraphicsItem*> items = scene->selectedItems();
	if (items.isEmpty())
		return;
	pasteOffset = OffsetIncrement;
	copyItems(items);
	updateUi();
}

void PageDesigner::editCut()
{
	QList<QGraphicsItem*> items = scene->selectedItems();
	if (items.isEmpty())
		return;
	copyItems(items);
	QListIterator<QGraphicsItem*> i(items);
	while (i.hasNext()) {
#if QT_VERSION >= 0x040600
		QScopedPointer<QGraphicsItem> item(i.next());
		scene->removeItem(item.data());
#else
		QGraphicsItem* item = i.next();
		scene->removeItem(item);
		delete item;
#endif
	}
	setDirty(true);
}

void PageDesigner::editPaste()
{
	QClipboard* clipboard = QApplication::clipboard();
	const QMimeData* mimeData = clipboard->mimeData();
	if (!mimeData)
		return;

	if (mimeData->hasFormat(MimeType)) {
		QByteArray copiedItems = mimeData->data(MimeType);
		QDataStream in(&copiedItems, QIODevice::ReadOnly);
		readItems(in, pasteOffset, true);
		pasteOffset += OffsetIncrement;
	}
	else if (mimeData->hasHtml() || mimeData->hasText()) {
		TextItem* textItem = new TextItem(position(), scene);
		connectItem(textItem);
		if (mimeData->hasHtml())
			textItem->setHtml(mimeData->html());
		else
			textItem->setPlainText(mimeData->text());
	}
	else
		return;
	setDirty(true);
}

void PageDesigner::editAlign()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (!action)
		return;

	Qt::Alignment alignment = static_cast<Qt::Alignment>(
		action->data().toInt());
	if (action != editAlignmentAction) {
		editAlignmentAction->setData(action->data());
		editAlignmentAction->setIcon(action->icon());
	}

	QList<QGraphicsItem*> items = scene->selectedItems();
	QVector<double> coordinates;
	populateCoordinates(alignment, &coordinates, items);
	double offset;
	if (alignment == Qt::AlignLeft || alignment == Qt::AlignTop)
		offset = *std::min_element(coordinates.constBegin(),
			coordinates.constEnd());
	else
		offset = *std::max_element(coordinates.constBegin(),
			coordinates.constEnd());

	if (alignment == Qt::AlignLeft || alignment == Qt::AlignRight) {
		for (int i = 0; i < items.count(); ++i)
			items.at(i)->moveBy(offset - coordinates.at(i), 0);
	}
	else {
		for (int i = 0; i < items.count(); ++i)
			items.at(i)->moveBy(0, offset - coordinates.at(i));
	}
	setDirty(true);
}

void PageDesigner::editClearTransforms()
{
	QList<QGraphicsItem*> items = scene->selectedItems();
	Q_ASSERT(!items.isEmpty());
	QListIterator<QGraphicsItem*> i(items);
	while (i.hasNext()) {
#ifdef NO_DYNAMIC_CAST
		if (QObject* item = qObjectFrom(i.next())) {
#else
		if (QObject* item = dynamic_cast<QObject*>(i.next())) {
#endif
			if (item->property("angle").isValid()) {
				item->setProperty("angle", 0.0);
				item->setProperty("shearHorizontal", 0.0);
				item->setProperty("shearVertical", 0.0);
			}
		}
		}
	transformWidget->setAngle(0.0);
	transformWidget->setShear(0.0, 0.0);
	setDirty(true);
}

void PageDesigner::updateUi()
{
	fileSaveAction->setEnabled(isWindowModified());
	bool hasItems = sceneHasItems();
	fileSaveAsAction->setEnabled(hasItems);
	fileExportAction->setEnabled(hasItems);
	filePrintAction->setEnabled(hasItems);
	int selected = scene->selectedItems().count();
	editSelectedItemAction->setEnabled(selected == 1);
	editCopyAction->setEnabled(selected >= 1);
	editCutAction->setEnabled(selected >= 1);
	QClipboard* clipboard = QApplication::clipboard();
	const QMimeData* mimeData = clipboard->mimeData();
	editPasteAction->setEnabled(mimeData &&
		(mimeData->hasFormat(MimeType) || mimeData->hasHtml() ||
			mimeData->hasText()));
	editAlignmentAction->setEnabled(selected >= 2);
	editClearTransformsAction->setEnabled(selected >= 1);
	transformWidget->setEnabled(selected >= 1);
	bool hasBrushProperty;
	bool hasPenProperty;
	getSelectionProperties(&hasBrushProperty, &hasPenProperty);
	brushWidget->setEnabled(hasBrushProperty);
	penWidget->setEnabled(hasPenProperty);
}

void PageDesigner::viewShowGrid(bool on)
{
	if (!gridGroup) {
		const int GridSize = 40;
		QPen pen(QColor(175, 175, 175, 127));
		gridGroup = new QGraphicsItemGroup;
		const int MaxX = static_cast<int>(std::ceil(scene->width())
			/ GridSize) * GridSize;
		const int MaxY = static_cast<int>(std::ceil(scene->height())
			/ GridSize) * GridSize;
		for (int x = 0; x <= MaxX; x += GridSize) {
			QGraphicsLineItem* item = new QGraphicsLineItem(x, 0, x,
				MaxY);
			item->setPen(pen);
			item->setZValue(std::numeric_limits<int>::min());
			gridGroup->addToGroup(item);
		}
		for (int y = 0; y <= MaxY; y += GridSize) {
			QGraphicsLineItem* item = new QGraphicsLineItem(0, y,
				MaxX, y);
			item->setPen(pen);
			item->setZValue(-1000);
			gridGroup->addToGroup(item);
		}
		scene->addItem(gridGroup);
	}
	gridGroup->setVisible(on);
}

void PageDesigner::selectionChanged()
{
	QList<QGraphicsItem*> items = scene->selectedItems();
	if (items.count() == 1) {
#ifdef NO_DYNAMIC_CAST
		if (QObject* item = qObjectFrom(items.at(0))) {
#else
		if (QObject* item = dynamic_cast<QObject*>(items.at(0))) {
#endif
			if (item->property("brush").isValid())
				brushWidget->setBrush(
					item->property("brush").value<QBrush>());
			if (item->property("pen").isValid())
				penWidget->setPen(
					item->property("pen").value<QPen>());
			if (item->property("angle").isValid()) {
				transformWidget->setAngle(
					item->property("angle").toDouble());
				transformWidget->setShear(
					item->property("shearHorizontal").toDouble(),
					item->property("shearVertical").toDouble());
			}
		}
		}
	updateUi();
}

void PageDesigner::loadFile()
{
	QFile file(windowFilePath());
	QDataStream in;
	if (!openPageDesignerFile(&file, in))
		return;
	in.setVersion(QDataStream::Qt_4_5);
	clear();
	readItems(in);
	statusBar()->showMessage(tr("Loaded %1").arg(windowFilePath()),
		StatusTimeout);
	setDirty(false);
}

void PageDesigner::createSceneAndView()
{
	view = new GraphicsView;
	scene = new QGraphicsScene(this);
	QSize pageSize = printer->paperSize(QPrinter::Point).toSize();
	scene->setSceneRect(0, 0, pageSize.width(), pageSize.height());
	view->setScene(scene);
	setCentralWidget(view);
}

void PageDesigner::createActions()
{
	fileNewAction = new QAction(QIcon(":/images/filenew.png"),
		tr("New..."), this);
	fileNewAction->setShortcuts(QKeySequence::New);
	fileOpenAction = new QAction(QIcon(":/images/fileopen.png"),
		tr("Open..."), this);
	fileOpenAction->setShortcuts(QKeySequence::Open);
	fileSaveAction = new QAction(QIcon(":/images/filesave.png"),
		tr("Save"), this);
	fileSaveAction->setShortcuts(QKeySequence::Save);
	fileSaveAsAction = new QAction(QIcon(":/images/filesave.png"),
		tr("Save As..."), this);
#if QT_VERSION >= 0x040500
	fileSaveAsAction->setShortcuts(QKeySequence::SaveAs);
#endif
	fileExportAction = new QAction(QIcon(":/images/fileexport.png"),
		tr("Export..."), this);
	filePrintAction = new QAction(QIcon(":/images/fileprint.png"),
		tr("Print..."), this);
	fileQuitAction = new QAction(QIcon(":/images/filequit.png"),
		tr("Quit"), this);
#if QT_VERSION >= 0x040600
	fileQuitAction->setShortcuts(QKeySequence::Quit);
#else
	fileQuitAction->setShortcut(QKeySequence("Ctrl+Q"));
#endif

	editSelectedItemAction = new QAction(
		QIcon(":/images/editselecteditem.png"),
		tr("Edit Selected Item..."), this);
	editAddTextAction = new QAction(QIcon(":/images/editaddtext.png"),
		tr("Add Text..."), this);
	editAddTextAction->setData(TextItemType);
	editAddBoxAction = new QAction(QIcon(":/images/editaddbox.png"),
		tr("Add Box"), this);
	editAddBoxAction->setData(BoxItemType);
	editAddSmileyAction = new QAction(QIcon(":/images/editaddsmiley.png"),
		tr("Add Smiley"), this);
	editAddSmileyAction->setData(SmileyItemType);
	editCopyAction = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"),
		this);
	editCopyAction->setShortcuts(QKeySequence::Copy);
	editCutAction = new QAction(QIcon(":/images/editcut.png"), tr("Cu&t"),
		this);
	editCutAction->setShortcuts(QList<QKeySequence>()
		<< QKeySequence::Cut << Qt::Key_Delete);
	editPasteAction = new QAction(QIcon(":/images/editpaste.png"),
		tr("&Paste"), this);
	editPasteAction->setShortcuts(QKeySequence::Paste);

	editAlignmentAction = new QAction(QIcon(":/images/align-left.png"),
		tr("Alignment"), this);
	editAlignmentAction->setData(Qt::AlignLeft);
	editAlignLeftAction = new QAction(QIcon(":/images/align-left.png"),
		tr("Left"), this);
	editAlignLeftAction->setData(Qt::AlignLeft);
	editAlignRightAction = new QAction(QIcon(":/images/align-right.png"),
		tr("Right"), this);
	editAlignRightAction->setData(Qt::AlignRight);
	editAlignTopAction = new QAction(QIcon(":/images/align-top.png"),
		tr("Top"), this);
	editAlignTopAction->setData(Qt::AlignTop);
	editAlignBottomAction = new QAction(QIcon(":/images/align-bottom.png"),
		tr("Bottom"), this);
	editAlignBottomAction->setData(Qt::AlignBottom);
	editClearTransformsAction = new
		QAction(QIcon(":/images/editcleartransforms.png"),
			tr("Clear Transformations"), this);

	viewZoomInAction = new QAction(QIcon(":/images/zoom-in.png"),
		tr("Zoom In"), this);
	viewZoomInAction->setShortcut(tr("+"));
	viewZoomOutAction = new QAction(QIcon(":/images/zoom-out.png"),
		tr("Zoom Out"), this);
	viewZoomOutAction->setShortcut(tr("-"));
	viewShowGridAction = new QAction(QIcon(":/images/showgrid.png"),
		tr("Show Grid"), this);
	viewShowGridAction->setCheckable(true);
	viewShowGridAction->setChecked(true);
}

void PageDesigner::createMenusAndToolBars()
{
	QAction* separator = 0;
	setUnifiedTitleAndToolBarOnMac(true);

	QMenu* fileMenu = menuBar()->addMenu(tr("File"));
	QToolBar* fileToolBar = addToolBar(tr("File"));
	populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()
		<< fileNewAction << fileOpenAction << fileSaveAction
		<< fileExportAction << separator << filePrintAction);
	fileMenu->insertAction(fileExportAction, fileSaveAsAction);
	fileMenu->addSeparator();
	fileMenu->addAction(fileQuitAction);

	QMenu* alignmentMenu = new QMenu(tr("Align"), this);
	foreach(QAction * action, QList<QAction*>()
		<< editAlignLeftAction << editAlignRightAction
		<< editAlignTopAction << editAlignBottomAction)
		alignmentMenu->addAction(action);
	editAlignmentAction->setMenu(alignmentMenu);

	QMenu* editMenu = menuBar()->addMenu(tr("Edit"));
	QToolBar* editToolBar = addToolBar(tr("Edit"));
	populateMenuAndToolBar(editMenu, editToolBar, QList<QAction*>()
		<< editSelectedItemAction << separator
		<< editAddTextAction << editAddBoxAction
		<< editAddSmileyAction << separator << editCopyAction
		<< editCutAction << editPasteAction << separator
		<< editAlignmentAction << editClearTransformsAction);

	QMenu* viewMenu = menuBar()->addMenu(tr("View"));
	QToolBar* viewToolBar = addToolBar(tr("View"));
	populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
		<< viewZoomInAction << viewZoomOutAction << separator
		<< viewShowGridAction);

	AQP::accelerateMenu(menuBar());
}

void PageDesigner::populateMenuAndToolBar(QMenu* menu, QToolBar* toolBar,
	QList<QAction*> actions)
{
	foreach(QAction * action, actions) {
		if (!action) {
			menu->addSeparator();
			toolBar->addSeparator();
		}
		else {
			menu->addAction(action);
			toolBar->addAction(action);
		}
	}
}

void PageDesigner::createDockWidgets()
{
	setDockOptions(QMainWindow::AnimatedDocks);
	QDockWidget::DockWidgetFeatures features =
		QDockWidget::DockWidgetMovable |
		QDockWidget::DockWidgetFloatable;

	transformWidget = new TransformWidget;
	QDockWidget* transformDockWidget = new QDockWidget(
		tr("Transform"), this);
	transformDockWidget->setFeatures(features);
	transformDockWidget->setWidget(transformWidget);
	addDockWidget(Qt::RightDockWidgetArea, transformDockWidget);

	brushWidget = new BrushWidget;
	QDockWidget* brushDockWidget = new QDockWidget(tr("Brush (Fill)"),
		this);
	brushDockWidget->setFeatures(features);
	brushDockWidget->setWidget(brushWidget);
	addDockWidget(Qt::RightDockWidgetArea, brushDockWidget);

	penWidget = new PenWidget;
	QDockWidget* penDockWidget = new QDockWidget(tr("Pen (Outline)"),
		this);
	penDockWidget->setFeatures(features);
	penDockWidget->setWidget(penWidget);
	addDockWidget(Qt::RightDockWidgetArea, penDockWidget);
}

void PageDesigner::createConnections()
{
	connect(fileNewAction, SIGNAL(triggered()),
		this, SLOT(fileNew()));
	connect(fileOpenAction, SIGNAL(triggered()),
		this, SLOT(fileOpen()));
	connect(fileSaveAction, SIGNAL(triggered()),
		this, SLOT(fileSave()));
	connect(fileSaveAsAction, SIGNAL(triggered()),
		this, SLOT(fileSaveAs()));
	connect(fileExportAction, SIGNAL(triggered()),
		this, SLOT(fileExport()));
	connect(filePrintAction, SIGNAL(triggered()),
		this, SLOT(filePrint()));
	connect(fileQuitAction, SIGNAL(triggered()),
		this, SLOT(close()));
	connect(editSelectedItemAction, SIGNAL(triggered()),
		this, SLOT(editSelectedItem()));
	connect(editAddTextAction, SIGNAL(triggered()),
		this, SLOT(editAddItem()));
	connect(editAddBoxAction, SIGNAL(triggered()),
		this, SLOT(editAddItem()));
	connect(editAddSmileyAction, SIGNAL(triggered()),
		this, SLOT(editAddItem()));
	connect(editCopyAction, SIGNAL(triggered()),
		this, SLOT(editCopy()));
	connect(editCutAction, SIGNAL(triggered()),
		this, SLOT(editCut()));
	connect(editPasteAction, SIGNAL(triggered()),
		this, SLOT(editPaste()));
	connect(QApplication::clipboard(), SIGNAL(dataChanged()),
		this, SLOT(updateUi()));
	foreach(QAction * action, QList<QAction*>()
		<< editAlignmentAction << editAlignLeftAction
		<< editAlignRightAction << editAlignTopAction
		<< editAlignBottomAction)
		connect(action, SIGNAL(triggered()), this, SLOT(editAlign()));
	connect(editClearTransformsAction, SIGNAL(triggered()),
		this, SLOT(editClearTransforms()));
	connect(scene, SIGNAL(selectionChanged()),
		this, SLOT(selectionChanged()));
	connect(viewShowGridAction, SIGNAL(toggled(bool)),
		this, SLOT(viewShowGrid(bool)));
	connect(viewZoomInAction, SIGNAL(triggered()),
		view, SLOT(zoomIn()));
	connect(viewZoomOutAction, SIGNAL(triggered()),
		view, SLOT(zoomOut()));
}

bool PageDesigner::okToClearData()
{
	if (isWindowModified())
		return AQP::okToClearData(&PageDesigner::fileSave, this,
			tr("Unsaved changes"), tr("Save unsaved changes?"));
	return true;
}

QPoint PageDesigner::position()
{
    QPoint point = mapFromGlobal(QCursor::pos());
    if (!view->geometry().contains(point)) {
        point = previousPoint.isNull()
                ? view->pos() + QPoint(10, 10) : previousPoint;
    }
    if (!previousPoint.isNull() && point == previousPoint) {
        point += QPoint(addOffset, addOffset);
        addOffset += OffsetIncrement;
    }
    else {
        addOffset = OffsetIncrement;
        previousPoint = point;
    }
    return view->mapToScene(point - view->pos()).toPoint();
}

void PageDesigner::clear()
{
	scene->clear();
	gridGroup = 0;
	viewShowGrid(viewShowGridAction->isChecked());
}

void PageDesigner::connectItem(QObject* item)
{
	connect(item, SIGNAL(dirty()), this, SLOT(setDirty()));
	const QMetaObject* metaObject = item->metaObject();
	if (metaObject->indexOfProperty("brush") > -1)
		connect(brushWidget, SIGNAL(brushChanged(const QBrush&)),
			item, SLOT(setBrush(const QBrush&)));
	if (metaObject->indexOfProperty("pen") > -1)
		connect(penWidget, SIGNAL(penChanged(const QPen&)),
			item, SLOT(setPen(const QPen&)));
	if (metaObject->indexOfProperty("angle") > -1) {
		connect(transformWidget, SIGNAL(angleChanged(double)),
			item, SLOT(setAngle(double)));
		connect(transformWidget, SIGNAL(shearChanged(double, double)),
			item, SLOT(setShear(double, double)));
	}
}

bool PageDesigner::openPageDesignerFile(QFile* file, QDataStream& in)
{
	if (!file->open(QIODevice::ReadOnly)) {
		AQP::warning(this, tr("Error"), tr("Failed to open file: %1")
			.arg(file->errorString()));
		return false;
	}
	in.setDevice(file);
	qint32 magicNumber;
	in >> magicNumber;
	if (magicNumber != MagicNumber) {
		AQP::warning(this, tr("Error"),
			tr("%1 is not a %2 file").arg(file->fileName())
			.arg(QApplication::applicationName()));
		return false;
	}
	qint16 versionNumber;
	in >> versionNumber;
	if (versionNumber > VersionNumber) {
		AQP::warning(this, tr("Error"),
			tr("%1 needs a more recent version of %2")
			.arg(file->fileName())
			.arg(QApplication::applicationName()));
		return false;
	}
	return true;
}

void PageDesigner::readItems(QDataStream& in, int offset, bool select)
{
	QSet<QGraphicsItem*> items;
	qint32 itemType;
	QGraphicsItem* item = 0;
	while (!in.atEnd()) {
		in >> itemType;
		switch (itemType) {
		case BoxItemType: {
			BoxItem* boxItem = new BoxItem(QRect(), scene);
			in >> *boxItem;
			connectItem(boxItem);
			item = boxItem;
			break;
		}
		case SmileyItemType: {
			SmileyItem* smileyItem = new SmileyItem(QPoint(),
				scene);
			in >> *smileyItem;
			connectItem(smileyItem);
			item = smileyItem;
			break;
		}
		case TextItemType: {
			TextItem* textItem = new TextItem(QPoint(), scene);
			in >> *textItem;
			connectItem(textItem);
			item = textItem;
			break;
		}
		}
		if (item) {
			item->moveBy(offset, offset);
			if (select)
				items << item;
			item = 0;
		}
	}
	if (select)
		selectItems(items);
	else
		selectionChanged();
}

void PageDesigner::writeItems(QDataStream& out, const QList<QGraphicsItem*>& items)
{
	foreach(QGraphicsItem * item, items) {
		if (item == gridGroup || item->group() == gridGroup)
			continue;
		qint32 type = static_cast<qint32>(item->type());
		out << type;
		switch (type) {
		case BoxItemType:
			out << *static_cast<BoxItem*>(item); break;
		case SmileyItemType:
			out << *static_cast<SmileyItem*>(item); break;
		case TextItemType:
			out << *static_cast<TextItem*>(item); break;
		default: Q_ASSERT(false);
		}
	}
}

void PageDesigner::copyItems(const QList<QGraphicsItem*>& items)
{
	QByteArray copiedItems;
	QDataStream out(&copiedItems, QIODevice::WriteOnly);
	writeItems(out, items);
	QMimeData* mimeData = new QMimeData;
	mimeData->setData(MimeType, copiedItems);
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setMimeData(mimeData);
}

void PageDesigner::selectItems(const QSet<QGraphicsItem*>& items)
{
	scene->clearSelection();
	foreach(QGraphicsItem * item, items)
		item->setSelected(true);
	selectionChanged();
}

void PageDesigner::alignItems(Qt::Alignment alignment)
{}

void PageDesigner::exportImage(const QString& filename)
{
	QImage image(printer->paperSize(QPrinter::Point).toSize(),
		QImage::Format_ARGB32);
	{
		QPainter painter(&image);
		painter.setRenderHints(QPainter::Antialiasing |
			QPainter::TextAntialiasing);
		paintScene(&painter);
	}
	if (image.save(filename))
		statusBar()->showMessage(tr("Exported %1").arg(filename),
			StatusTimeout);
	else
		AQP::warning(this, tr("Error"), tr("Failed to export: %1")
			.arg(filename));
}

void PageDesigner::exportSvg(const QString& filename)
{
	QSvgGenerator svg;
	svg.setFileName(filename);
	svg.setSize(printer->paperSize(QPrinter::Point).toSize());
	{
		QPainter painter(&svg);
		paintScene(&painter);
	}
	statusBar()->showMessage(tr("Exported %1").arg(filename),
		StatusTimeout);
}

void PageDesigner::paintScene(QPainter* painter)
{
	bool showGrid = viewShowGridAction->isChecked();
	if (showGrid)
		viewShowGrid(false);
	QList<QGraphicsItem*> items = scene->selectedItems();
	scene->clearSelection();

	scene->render(painter);

	if (showGrid)
		viewShowGrid(true);
	foreach(QGraphicsItem * item, items)
		item->setSelected(true);
	selectionChanged();
}

void PageDesigner::populateCoordinates(const Qt::Alignment& alignment,
	QVector<double>* coordinates, const QList<QGraphicsItem*>& items)
{
	QListIterator<QGraphicsItem*> i(items);
	while (i.hasNext()) {
		QRectF rect = i.next()->sceneBoundingRect();
		switch (alignment) {
		case Qt::AlignLeft:
			coordinates->append(rect.x()); break;
		case Qt::AlignRight:
			coordinates->append(rect.x() + rect.width()); break;
		case Qt::AlignTop:
			coordinates->append(rect.y()); break;
		case Qt::AlignBottom:
			coordinates->append(rect.y() + rect.height()); break;
		}
	}
}

bool PageDesigner::sceneHasItems() const
{
	foreach(QGraphicsItem * item, scene->items())
		if (item != gridGroup && item->group() != gridGroup)
			return true;
	return false;
}

void PageDesigner::getSelectionProperties(bool* hasBrushProperty,
	bool* hasPenProperty) const
{
	Q_ASSERT(hasBrushProperty && hasPenProperty);
	*hasBrushProperty = false;
	*hasPenProperty = false;
	foreach(QGraphicsItem * item, scene->selectedItems()) {
#ifdef NO_DYNAMIC_CAST
		if (QObject* object = qObjectFrom(item)) {
#else
		if (QObject* object = dynamic_cast<QObject*>(item)) {
#endif
			const QMetaObject* metaObject = object->metaObject();
			if (metaObject->indexOfProperty("brush") > -1)
				*hasBrushProperty = true;
			if (metaObject->indexOfProperty("pen") > -1)
				*hasPenProperty = true;
			if (*hasBrushProperty && *hasPenProperty)
				break;
		}
	}
}
