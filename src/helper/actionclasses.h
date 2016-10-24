#ifndef ACTIONCLASSES_H
#define ACTIONCLASSES_H

#include <QAction>

class IntValueAction : public QAction
{
	Q_OBJECT
	int valTrue;
public:
	IntValueAction(const int t, QObject* parent) : QAction(parent), valTrue(t)
	{
		setCheckable(true);
		connect(this, &QAction::toggled, this, &IntValueAction::onTriggered);
	}
signals:
	void triggered(int v);
	
private slots:
	void onTriggered(bool v)
	{
		if(v)
			emit(triggered(valTrue));
	}
public slots:
	void valueChanged(int v)
	{
		if(v == valTrue)
			setChecked(true);
		else
			setChecked(false);
	}
};

#endif // ACTIONCLASSES_H
