#ifndef INFODIALOGS_H
#define INFODIALOGS_H

#include<QObject>

class QWidget;

class InfoDialogs : public QObject
{
	Q_OBJECT
public:
	static void showAboutDialog(QWidget* parent);
};

#endif // INFODIALOGS_H
