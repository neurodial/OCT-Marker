/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
