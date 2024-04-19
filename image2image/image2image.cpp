#include "image2image.h"

#include "../aqp/aqp.hpp"
#include "../aqp/alt_key.hpp"

#include <QApplication>
#include <QCloseEvent>
#include <QCompleter>
#include <QComboBox>
#include <QDirIterator>
#include <QDirModel>
#include <QGridLayout>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThreadPool>
#include <QTimer>
#include <QtConcurrent>

const int PollTimeout = 100;

class DirModel : public QDirModel
{
public:
	explicit DirModel(QObject* parent = nullptr)
		:QDirModel(parent)
	{}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
	{
		if (Qt::DisplayRole == role && 0 == index.column())
		{
			QString path = QDir::toNativeSeparators(filePath(index));
			if (path.endsWith(QDir::separator())){
				path.chop(1);
			}

			return path;
		}

		return QDirModel::data(index, role);
	}
};


struct ProgressEvent : public QEvent
{
	enum { EventId = QEvent::User };

	explicit ProgressEvent(bool saved_, const QString& message_)
		: QEvent(static_cast<Type>(EventId)),
		saved(saved_), message(message_) {}

	const bool saved;
	const QString message;
};

void convertImages(QObject* receiver, volatile bool* stopped,
	const QStringList& sourceFiles, const QString& targetType)
{
	foreach(const QString & source, sourceFiles) {
		if (*stopped)
			return;
		QImage image(source);
		QString target(source);
		target.chop(QFileInfo(source).suffix().length());
		target += targetType.toLower();
		if (*stopped)
			return;
		bool saved = image.save(target);

		QString message = saved
			? QObject::tr("Saved '%1'")
			.arg(QDir::toNativeSeparators(target))
			: QObject::tr("Failed to convert '%1'")
			.arg(QDir::toNativeSeparators(source));
		QApplication::postEvent(receiver,
			new ProgressEvent(saved, message));
	}
}

Image2Image::Image2Image(QWidget *parent)
	: QMainWindow(parent)
{
	createWidgets();
	createLayout();
	createConnections();

	AQP::accelerateWidget(this);
	updateUi();
	m_directoryEdit->setFocus();
	setWindowTitle(QApplication::applicationName());
}

Image2Image::~Image2Image()
{}

void Image2Image::convertOrCancel()
{
	m_stopped = true;
	if (QThreadPool::globalInstance()->activeThreadCount())
		QThreadPool::globalInstance()->waitForDone();
	if (m_convertOrCancelButton->text() == tr("&Cancel")) {
		updateUi();
		return;
	}

	QString sourceType = m_sourceTypeComboBox->currentText();
	QStringList sourceFiles;
	QDirIterator i(m_directoryEdit->text(), QDir::Files | QDir::Readable);
	while (i.hasNext()) {
		const QString& filenameAndPath = i.next();
		if (i.fileInfo().suffix().toUpper() == sourceType)
			sourceFiles << filenameAndPath;
	}
	if (sourceFiles.isEmpty())
		AQP::warning(this, tr("No Images Error"),
			tr("No matching files found"));
	else {
		m_logEdit->clear();
		convertFiles(sourceFiles);
	}
}

bool Image2Image::event(QEvent* event)
{
	if (!m_stopped && event->type() ==
		static_cast<QEvent::Type>(ProgressEvent::EventId)) {
		ProgressEvent* progressEvent =
			static_cast<ProgressEvent*>(event);
		Q_ASSERT(progressEvent);
		m_logEdit->appendPlainText(progressEvent->message);
		if (progressEvent->saved)
			++m_done;
		return true;
	}

	return QMainWindow::event(event);
}

void Image2Image::closeEvent(QCloseEvent* event)
{
	m_stopped = true;
	if (QThreadPool::globalInstance()->activeThreadCount())
		QThreadPool::globalInstance()->waitForDone();
	event->accept();
}

void Image2Image::updateUi()
{
	if (m_stopped) {
		m_convertOrCancelButton->setText(tr("&Convert"));
		m_convertOrCancelButton->setEnabled(
			!m_directoryEdit->text().isEmpty());
	}
	else {
		m_convertOrCancelButton->setText(tr("&Cancel"));
		m_convertOrCancelButton->setEnabled(true);
	}
}

void Image2Image::checkIfDone()
{
	if (QThreadPool::globalInstance()->activeThreadCount())
		QTimer::singleShot(PollTimeout, this, SLOT(checkIfDone()));
	else {
		QString message;
		if (m_done == m_total)
			message = tr("All %n image(s) converted", "", m_done);
		else
			message = tr("Converted %n/%1 image(s)", "", m_done)
			.arg(m_total);
		m_logEdit->appendPlainText(message);
		m_stopped = true;
		updateUi();
	}
}

void Image2Image::announceProgress(bool saved, const QString& message)
{

}

void Image2Image::sourceTypeChanged(const QString& sourceType)
{
	QStringList targetTypes;
	for (const auto& ba : QImageWriter::supportedImageFormats())
	{
		const QString targetType = QString(ba).toUpper();
		if (targetType != sourceType) {
			targetTypes << targetType.toUpper();
		}
	}

	targetTypes.sort();
	m_targetTypeComboBox->clear();
	m_targetTypeComboBox->addItems(targetTypes);
}

void Image2Image::createWidgets()
{
	//路径
	m_directoryLabel = new QLabel(tr("Path:"));
	
	QCompleter* directoryCompleter = new QCompleter(this);
	directoryCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	directoryCompleter->setModel(new DirModel(directoryCompleter));

	m_directoryEdit = new QLineEdit(QDir::toNativeSeparators(QDir::homePath()));
	m_directoryEdit->setCompleter(directoryCompleter);
	m_directoryLabel->setBuddy(m_directoryEdit);

	//源类型
	m_sourceTypeLabel = new QLabel(tr("Souce type:"));
	m_sourceTypeComboBox = new QComboBox;
	for (const auto& ba : QImageReader::supportedImageFormats()){
		m_sourceTypeComboBox->addItem(QString(ba.toUpper()));
	}
	m_sourceTypeLabel->setBuddy(m_sourceTypeComboBox);

	//目标类型
	m_targetTypeLabel = new QLabel(tr("Target type:"));
	m_targetTypeComboBox = new QComboBox;
	m_targetTypeLabel->setBuddy(m_targetTypeComboBox);
	sourceTypeChanged(m_sourceTypeComboBox->currentText());

	//日志
	m_logEdit = new QPlainTextEdit();
	m_logEdit->setReadOnly(true);
	m_logEdit->setPlainText(tr("Choose a path, source type and target "
		"file type, and click Convert."));
	
	//转换
	m_convertOrCancelButton = new QPushButton(tr("&Convert"));

	//退出
	m_quitButton = new QPushButton(tr("Quit"));
}

void Image2Image::createLayout()
{
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(m_directoryLabel, 0, 0);
	layout->addWidget(m_directoryEdit, 0, 1, 1, 5);
	layout->addWidget(m_sourceTypeLabel, 1, 0);
	layout->addWidget(m_sourceTypeComboBox, 1, 1);
	layout->addWidget(m_targetTypeLabel, 1, 2);
	layout->addWidget(m_targetTypeComboBox, 1, 3);
	layout->addWidget(m_convertOrCancelButton, 1, 4);
	layout->addWidget(m_quitButton, 1, 5);
	layout->addWidget(m_logEdit, 2, 0, 1, 6);

	QWidget* widget = new QWidget;
	widget->setLayout(layout);
	setCentralWidget(widget);
}

void Image2Image::createConnections()
{
	connect(m_directoryEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(updateUi()));
	connect(m_sourceTypeComboBox,
		SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(sourceTypeChanged(const QString&)));
	connect(m_sourceTypeComboBox, SIGNAL(activated(const QString&)),
		this, SLOT(sourceTypeChanged(const QString&)));
	connect(m_convertOrCancelButton, SIGNAL(clicked()),
		this, SLOT(convertOrCancel()));
	connect(m_quitButton, SIGNAL(clicked()), this, SLOT(close()));
}

void Image2Image::convertFiles(const QStringList& sourceFiles)
{
	m_stopped = false;
	updateUi();
	m_total = sourceFiles.count();
	m_done = 0;
	const QVector<int> sizes = AQP::chunkSizes(sourceFiles.count(),
		QThread::idealThreadCount());

	int offset = 0;
	foreach(const int chunkSize, sizes) {
		QtConcurrent::run(convertImages, this, &m_stopped,
			sourceFiles.mid(offset, chunkSize),
			m_targetTypeComboBox->currentText());
		offset += chunkSize;
	}
	checkIfDone();
}
