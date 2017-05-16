#ifndef DWDEBUGOUTPUT_H
#define DWDEBUGOUTPUT_H

#include<QDockWidget>

class QTextEdit;
class QAbstractButton;
class QDialogButtonBox;

class Q_DebugStream;

class DWDebugOutput : public QDockWidget
{
	Q_OBJECT

	QTextEdit*     debugMessages;
	Q_DebugStream* debugStreamCout;
	Q_DebugStream* debugStreamCerr;
	QDialogButtonBox* buttonBox;
public:
	explicit DWDebugOutput(QWidget* parent = nullptr);
	virtual ~DWDebugOutput();


protected:
	void saveFile(const QString& filename);
	void saveFileAs();

signals:
	void messageFromOtherThread(QString msg);

public slots:
	void printMessages(QtMsgType type, const QMessageLogContext& context, const QString& msg);
	void printMessage(QString msg);

private slots:
	void printMessageLocalThread(QString msg);

	void clickButtonSlot(QAbstractButton* button);

};

#endif // DWDEBUGOUTPUT_H
