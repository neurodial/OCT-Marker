#include "bscanqualitymarker.h"


#include <QIcon>
#include <QColor>
#include <QPixmap>

namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
	}
}
