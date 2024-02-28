#pragma once

#include <QLineEdit>

class CommandLine  : public QLineEdit
{
	Q_OBJECT

public:
	CommandLine(QWidget *parent = 0);
	~CommandLine();

	QString getLastCommand();
	void appendCommand(const QString& cmd);
	QStringList getHistory() const;
	void setHistory(QStringList& h);
	void triggerCommand(const QString& cmd) {
		emit commandConfirmed(cmd);
	}

private:
	void createConnections();

signals:
	void clearHistory();
	void commandConfirmed(const QString& command);
	void completeCommand(const QString& command);
	void escape();

public slots:
	void paste();

protected:
	virtual void keyPressEvent(QKeyEvent* event);
	virtual bool event(QEvent* event);

private:
	QStringList history;
	QStringList::iterator it;
};
