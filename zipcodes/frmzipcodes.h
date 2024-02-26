#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_frmzipcodes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FrmZipCodesClass; };
QT_END_NAMESPACE

class QComboBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTableView;
class StandardTableModel;
class ProxyModel;
class FrmZipCodes : public QMainWindow
{
    Q_OBJECT

public:
    FrmZipCodes(QWidget *parent = nullptr);
    ~FrmZipCodes();

protected:
	void closeEvent(QCloseEvent* event);

private Q_SLOTS:
	void load();
	void load(const QString& filename);
	bool save();
	void addZipcode();
	void deleteZipcode();
	void setDirty() { setWindowModified(true); }
	void updateUi();
	void radioButtonClicked();
	void selectionChanged();

private:
	void createWidgets();
	void createComboBoxModels();
	void createComboBoxModel(QComboBox* comboBox, int column);
	void createLayout();
	void createConnections();
	bool okToClearData();
	void enableButtons(bool enable = true);
	bool matchingColumn(const QString& value, int row, int column);
	void restoreFilters();
	void reportFilterEffect();
	void performSelection();

	QDialogButtonBox* buttonBox;
	QPushButton* loadButton;
	QPushButton* saveButton;
	QPushButton* addButton;
	QPushButton* deleteButton;
	QPushButton* quitButton;
	QGroupBox* filterSelectGroupBox;
	QRadioButton* dontFilterOrSelectRadioButton;
	QRadioButton* filterRadioButton;
	QRadioButton* selectByCriteriaRadioButton;
	QLabel* minimumZipLabel;
	QSpinBox* minimumZipSpinBox;
	QLabel* maximumZipLabel;
	QSpinBox* maximumZipSpinBox;
	QGroupBox* countyGroupBox;
	QComboBox* countyComboBox;
	QGroupBox* stateGroupBox;
	QComboBox* stateComboBox;
	QTableView* tableView;

#ifdef CUSTOM_MODEL
	TableModel* model;
#else
	StandardTableModel* model;
#endif
	ProxyModel* proxyModel;
	bool loading;

private:
    Ui::FrmZipCodesClass *ui;
};
