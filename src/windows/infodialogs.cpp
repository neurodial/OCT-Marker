#include "infodialogs.h"


#include<QMessageBox>
#include<buildconstants.h>

void InfoDialogs::showAboutDialog(QWidget* parent)
{
	QString text("<h1><b>Info &#252;ber OCT-Marker</b></h1>");

	text += "<br /><center><b>nicht für den klinischen Gebrauch bestimmt<br />not for clinical usage</b></center>";
	text += "<br /><br />Author: Kay Gawlik";
	text += "<br /><br />Icons von <a href=\"http://www.famfamfam.com/lab/icons/silk/\">FamFamFam</a>";
	text += " und <a href=\"http://s-ings.com/typicons/\">Typicons</a>";

	text += "<br/><br/><b>Build-Informationen</b><table>";
	text += QString("<tr><td>&#220;bersetzungszeit </td><td> %1 %2</td></tr>").arg(BuildConstants::buildDate).arg(BuildConstants::buildTime);
	text += QString("<tr><td>Qt-Version </td><td> %1</td></tr>").arg(qVersion());
	text += QString("<tr><td>Git-Hash   </td><td> %1</td></tr>").arg(BuildConstants::gitSha1);
	text += QString("<tr><td>Build-Typ  </td><td> %1</td></tr>").arg(BuildConstants::buildTyp);
	text += QString("<tr><td>Compiler   </td><td> %1 %2</td></tr>").arg(BuildConstants::compilerId).arg(BuildConstants::compilerVersion);
	text += "</table>";

	QMessageBox::about(parent, tr("About"), text);
}
