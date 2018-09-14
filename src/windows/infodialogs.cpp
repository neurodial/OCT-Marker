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

#include "infodialogs.h"

#include<QApplication>
#include<QIcon>
#include<QMessageBox>
#include<QHBoxLayout>
#include<QVBoxLayout>

#include<QFile>
#include<QTextEdit>

#include<buildconstants.h>


AboutDialog::AboutDialog()
{
	setupUi(this);
	QIcon appIcon = QIcon(":/icons/typicons/oct_marker_logo.svg");

	setWindowIcon(appIcon);
	labelIcon->setPixmap(appIcon.pixmap(QSize(60, 60)));

	QString text("<h1><b>Info &#252;ber OCT-Marker</b></h1>");

	text += "<br /><center><b>nicht für den klinischen Gebrauch bestimmt<br />not for clinical usage</b></center>";
	text += "<br /><br />Author: Kay Gawlik";
	text += "<br /><br />Icons von <a href=\"http://www.famfamfam.com/lab/icons/silk/\">FamFamFam</a>";
	text += ", <a href=\"http://s-ings.com/typicons/\">Typicons</a>";
	text += " und dem <a href=\"http://tango.freedesktop.org/\">Tango-Desktop-Project</a>";

	text += "<br/><br/><b>Build-Informationen</b><table>";
	text += QString("<tr><td>&#220;bersetzungszeit </td><td> %1 %2</td></tr>").arg(BuildConstants::buildDate).arg(BuildConstants::buildTime);
	text += QString("<tr><td>Qt-Version </td><td> %1</td></tr>").arg(qVersion());
	text += QString("<tr><td>Git-Hash   </td><td> %1</td></tr>").arg(BuildConstants::gitSha1);
	text += QString("<tr><td>Build-Typ  </td><td> %1</td></tr>").arg(BuildConstants::buildTyp);
	text += QString("<tr><td>Compiler   </td><td> %1 %2</td></tr>").arg(BuildConstants::compilerId).arg(BuildConstants::compilerVersion);
	text += "</table>";

	labelAboutText->setText(text);
	labelAboutText->setTextInteractionFlags(Qt::TextBrowserInteraction);
	labelAboutText->setOpenExternalLinks(true);

	connect(pushButtonLicense, &QAbstractButton::clicked, this, &AboutDialog::showLicense);

	setWindowFlags(Qt::Tool);
}

void AboutDialog::showLicense()
{
	QFile file(":/license.txt");
	if(file.open(QIODevice::ReadOnly))
	{
		QDialog licenseDialog;
		licenseDialog.setWindowTitle(tr("License text"));

		QTextEdit* licenseText = new QTextEdit(&licenseDialog);
		licenseText->setReadOnly(true);
		licenseText->setText(file.readAll());

		QHBoxLayout* layout = new QHBoxLayout(&licenseDialog);
		layout->addWidget(licenseText);

		licenseDialog.setLayout(layout);
		licenseDialog.resize(550, licenseDialog.height());
		licenseDialog.exec();
	}
	else
		QMessageBox::critical(this, tr("License text"), tr("Missing license file"));
}

void InfoDialogs::showAboutDialog(QWidget* /*parent*/)
{
	AboutDialog test;

	test.exec();
}


