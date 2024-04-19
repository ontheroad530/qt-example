#pragma once

#include <QMainWindow>

#include <atomic>

class QCloseEvent;
class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

class Image2Image  : public QMainWindow
{
	Q_OBJECT

public:
	Image2Image(QWidget *parent = nullptr);
	~Image2Image();

private Q_SLOTS:
	void convertOrCancel();
	void updateUi();
	void checkIfDone();
	void announceProgress(bool saved, const QString& message);
	void sourceTypeChanged(const QString& sourceType);

protected:
	bool event(QEvent* event);
	void closeEvent(QCloseEvent* event);

private:
	void createWidgets();
	void createLayout();
	void createConnections();
	void convertFiles(const QStringList& sourceFiles);

	static void convertImages(QObject* receiver, const QStringList& sourceFiles, const QString& targetType);

private:
	QLabel*			m_directoryLabel = nullptr;
	QLineEdit*		m_directoryEdit = nullptr;
	QLabel*			m_sourceTypeLabel = nullptr;
	QComboBox*		m_sourceTypeComboBox = nullptr;
	QLabel*			m_targetTypeLabel = nullptr;
	QComboBox*		m_targetTypeComboBox = nullptr;
	QPlainTextEdit* m_logEdit = nullptr;
	QPushButton*	m_convertOrCancelButton = nullptr;
	QPushButton*	m_quitButton = nullptr;

	int				m_total = 0;
	int				m_done = 0;
	//volatile bool	m_stopped = true;
	std::atomic_bool	m_stopped = true;
};
