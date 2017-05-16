#ifndef DWDEBUGOUTPUT_H
#define DWDEBUGOUTPUT_H

#include<QDockWidget>

class QTextEdit;
class Q_DebugStream;

class DWDebugOutput : public QDockWidget
{
	QTextEdit*     textOutput;
	Q_DebugStream* debugStreamCout;
	Q_DebugStream* debugStreamCerr;
	Q_OBJECT
public:
	explicit DWDebugOutput(QWidget* parent = nullptr);
	virtual ~DWDebugOutput();
};

#endif // DWDEBUGOUTPUT_H
