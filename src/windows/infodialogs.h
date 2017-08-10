#ifndef INFODIALOGS_H
#define INFODIALOGS_H

#include<QObject>

class QWidget;

class InfoDialogs : public QObject
{
public:
	static void showAboutDialog(QWidget* parent);
};

#endif // INFODIALOGS_H
