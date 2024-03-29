#pragma once

#include <QPrinter>
#include <QtWidgets/QMainWindow>

class BrushWidget;
class GraphicsView;
class PenWidget;
class TransformWidget;
class QAction;
class QFile;
class QGraphicsItem;
class QGraphicsItemGroup;
class QGraphicsScene;

class PageDesigner : public QMainWindow
{
    Q_OBJECT

public:
    PageDesigner(QWidget *parent = nullptr);
    ~PageDesigner();

	QSize sizeHint() const;

public slots:
	void setDirty(bool on = true);

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void fileNew();
	void fileOpen();
	bool fileSave();
	bool fileSaveAs();
	void fileExport();
	void filePrint();
	void editSelectedItem();
	void editAddItem();
	void editCopy();
	void editCut();
	void editPaste();
	void editAlign();
	void editClearTransforms();
	void updateUi();
	void viewShowGrid(bool on);
	void selectionChanged();
	void loadFile();

private:
	void createSceneAndView();
	void createActions();
	void createMenusAndToolBars();
	void populateMenuAndToolBar(QMenu* menu, QToolBar* toolBar,
		QList<QAction*> actions);
	void createDockWidgets();
	void createConnections();
	bool okToClearData();
	QPoint position();
	void clear();
	void connectItem(QObject* item);
	bool openPageDesignerFile(QFile* file, QDataStream& in);
	void readItems(QDataStream& in, int offset = 0, bool select = false);
	void writeItems(QDataStream& out,
		const QList<QGraphicsItem*>& items);
	void copyItems(const QList<QGraphicsItem*>& items);
	void selectItems(const QSet<QGraphicsItem*>& items);
	void alignItems(Qt::Alignment alignment);
	void exportImage(const QString& filename);
	void exportSvg(const QString& filename);
	void paintScene(QPainter* painter);
	void populateCoordinates(const Qt::Alignment& alignment,
		QVector<double>* coordinates,
		const QList<QGraphicsItem*>& items);
	bool sceneHasItems() const;
	void getSelectionProperties(bool* hasBrushProperty,
		bool* hasPenProperty) const;

private:
	QAction* fileNewAction;
	QAction* fileOpenAction;
	QAction* fileSaveAction;
	QAction* fileSaveAsAction;
	QAction* fileExportAction;
	QAction* filePrintAction;
	QAction* fileQuitAction;
	QAction* editSelectedItemAction;
	QAction* editAddTextAction;
	QAction* editAddBoxAction;
	QAction* editAddSmileyAction;
	QAction* editCopyAction;
	QAction* editCutAction;
	QAction* editPasteAction;
	QAction* editAlignmentAction;
	QAction* editAlignLeftAction;
	QAction* editAlignRightAction;
	QAction* editAlignTopAction;
	QAction* editAlignBottomAction;
	QAction* editClearTransformsAction;
	QAction* viewZoomInAction;
	QAction* viewZoomOutAction;
	QAction* viewShowGridAction;

	TransformWidget* transformWidget;
	BrushWidget* brushWidget;
	PenWidget* penWidget;

	QPrinter* printer;
	QGraphicsScene* scene;
	GraphicsView* view;
	QGraphicsItemGroup* gridGroup;
	QPoint previousPoint;
	int addOffset;
	int pasteOffset;
};
