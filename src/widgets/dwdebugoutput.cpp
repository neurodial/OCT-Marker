#include "dwdebugoutput.h"

#include<iostream>

#include<QTextEdit>
#include<QTime>
#include<QThread>
#include<QVBoxLayout>
#include<QDialogButtonBox>

#include <QAbstractButton>
#include <QFileDialog>

#include<helper/qdebugstream.h>
#include <qcoreapplication.h>

DWDebugOutput::DWDebugOutput(QWidget* parent)
: QDockWidget(parent)
, debugMessages(new QTextEdit(this))
, debugStreamCout(new Q_DebugStream(std::cout, this, "cout"))
, debugStreamCerr(new Q_DebugStream(std::cerr, this, "cerr"))
{
	setWindowTitle(tr("Debug output"));
	setWidget(debugMessages);

	debugMessages->setReadOnly(true);
	debugMessages->setUndoRedoEnabled(false);
	debugMessages->setAcceptRichText(false);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setStandardButtons(QDialogButtonBox::Reset | QDialogButtonBox::Save);

	QVBoxLayout* boxlayout = new QVBoxLayout(this);
	boxlayout->addWidget(debugMessages);
	boxlayout->addWidget(buttonBox);


	QWidget* newWidget = new QWidget(this);
	newWidget->setLayout(boxlayout);
	setWidget(newWidget);


	connect(this, &DWDebugOutput::messageFromOtherThread, this, &DWDebugOutput::printMessageLocalThread);
	connect(buttonBox, &QDialogButtonBox::clicked, this, &DWDebugOutput::clickButtonSlot);

}


DWDebugOutput::~DWDebugOutput()
{
	delete debugStreamCout;
	delete debugStreamCerr;
}

void DWDebugOutput::printMessages(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QTime time = QTime::currentTime();
	QString messages = time.toString(); // QString("%1:%2:%3 :").arg(time.hour()).arg(time.minute()).arg(time.second());

	switch(type)
	{
	case QtInfoMsg:
		messages += QString(" Info: %1").arg(msg);
		break;
	case QtDebugMsg:
		messages += QString(" Debug: %1").arg(msg);
		break;
	case QtWarningMsg:
		messages += QString(" Warning: %1").arg(msg);
		break;
	case QtCriticalMsg:
		messages += QString(" Critical: %1").arg(msg);
		break;
	case QtFatalMsg:
		messages += QString(" Fatal: %1").arg(msg);
		break;
	}

// 	std::cout << context.category  << '\t'  << context.file << '\t'<< context.function  << '\t' << context.line  << '\t' << context.version << '\n';
// 	std::cout << messages.toStdString() << std::endl;
//
    const bool isGuiThread = (QThread::currentThread() == QCoreApplication::instance()->thread());
	if(isGuiThread)
		debugMessages->append(messages);
	else
		emit(messageFromOtherThread(messages));
}


void DWDebugOutput::printMessage(QString msg)
{
	QTime time = QTime::currentTime();
	QString messages = time.toString() + " " + msg;

    const bool isGuiThread = (QThread::currentThread() == QCoreApplication::instance()->thread());
	if(isGuiThread)
		debugMessages->append(messages);
	else
		emit(messageFromOtherThread(messages));
}

void DWDebugOutput::printMessageLocalThread(QString msg)
{
	debugMessages->append(msg);
}



void DWDebugOutput::clickButtonSlot(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton stdButton = buttonBox->standardButton(button);

	switch(stdButton)
	{
		case QDialogButtonBox::Reset:
			debugMessages->clear();
			break;
		case QDialogButtonBox::Save:
			saveFileAs();
			break;
		default:
			break;
	//	case QDialogButtonBox::Close:
	//		this->close();
	//		break;
	}
}

void DWDebugOutput::saveFile(const QString& filename)
{
	QFile file(filename);
	if(file.open(QIODevice::WriteOnly|QIODevice::Text))
	{
		file.write(debugMessages->toPlainText().toUtf8());
	}
}

void DWDebugOutput::saveFileAs()
{
	QString filePath = QFileDialog::getSaveFileName(this,tr("save debug messages"),QString(),"text files (*.txt)");
	if(filePath.isEmpty())
		return;
	saveFile(filePath);
}

