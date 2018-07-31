#include "programoptionstype.h"
#include<QColorDialog>
#include<QInputDialog>
#include<QLineEdit>

#include <data_structure/programoptions.h>


Option::Option(const QString& name, const QString& optClass)
: name    (name    )
, optClass(optClass)
{
	ProgramOptions::registerOption(this);
}

void OptionColor::showColorDialog()
{
	QColorDialog dialog;
	dialog.setCurrentColor(value);
	if(dialog.exec())
	{
		value = dialog.currentColor();
		emit(valueChanged(value));
	}
}


void OptionInt::showInputDialog()
{
	bool ok;
	int v = QInputDialog::getInt(nullptr, dialogTitle, dialogText, value, 0, 20, 1, &ok);
	if(ok)
		value = v;
}
