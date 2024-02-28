#include "commandline.h"

#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>

CommandLine::CommandLine(QWidget *parent)
	: QLineEdit(parent)
	, it(history.end())
{
	//for test
	createConnections();
}

CommandLine::~CommandLine()
{}

QString CommandLine::getLastCommand()
{
	if (history.isEmpty()) {
		return QString();
	}
	return history.last();
}

void CommandLine::appendCommand(const QString& cmd)
{
	if (!cmd.isEmpty() && (history.isEmpty() || history.last() != cmd)) {
		history.append(cmd);
	}
	it = history.end();
}

QStringList CommandLine::getHistory() const
{
	return history;
}

void CommandLine::setHistory(QStringList& h)
{
	history = h;
	it = history.end();
}

void CommandLine::createConnections()
{
	connect(this, &CommandLine::clearHistory, this, [this]() {history.clear(); it = history.end(); });
	connect(this, &CommandLine::commandConfirmed, this, &CommandLine::appendCommand);
}

void CommandLine::paste()
{
	QClipboard* cb = QApplication::clipboard();

	QString text = cb->text();

	// multi line paste and enter:
	if (text.contains("\n")) {
		QStringList lines = text.split('\n');
		for (int i = 0; i < lines.length(); i++) {
			//qDebug("line: " + lines[i]);
			emit commandConfirmed(lines[i]);
		}
	}
	else {
		// single line paste only:
		QLineEdit::paste();
	}
}

void CommandLine::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_L:
		if (event->modifiers() == Qt::ControlModifier) {
			emit clearHistory();
			return;
		}
		break;
	case Qt::Key_V:
		if (event->modifiers() == Qt::ControlModifier) {
			paste();
			return;
		}
		break;
	case Qt::Key_Enter:
	case Qt::Key_Return: {
		QString t = text();
		//if (!t.isEmpty() && (history.isEmpty() || history.last() != t)) {
		//    history.append(t);
		//}
		it = history.end();
		emit commandConfirmed(t);
	}
		break;
	case Qt::Key_Up:
		if (it != history.begin()) {
			it--;
			setText(*it);
		}
		return;
		break;
	case Qt::Key_Down:
		if (it != history.end()) {
			it++;
			if (it != history.end()) {
				setText(*it);
			}
			else {
				clear();
			}
		}
		return;
		break;
	default:
		break;
	}
	QLineEdit::keyPressEvent(event);
}

bool CommandLine::event(QEvent* event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* ke = dynamic_cast<QKeyEvent*> (event);
		if (ke->key() == Qt::Key_Tab) {
			emit completeCommand(text());
			return true;
		}
	}
	return QLineEdit::event(event);
}
