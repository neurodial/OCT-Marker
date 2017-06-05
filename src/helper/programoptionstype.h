#ifndef PROGRAMOPTIONSTYPE_H
#define PROGRAMOPTIONSTYPE_H

// #include <QObject>
#include <QVariant>
#include <QColor>
#include <sys/stat.h>
#include <QAction>

class ProgramOptions;

class Option : public QObject
{
	Q_OBJECT
	
	QString name;
public:
	Option(const QString& name);
	virtual ~Option() {}
	virtual void resetValue() = 0;
	const QString& getName() const { return name; }
	
	virtual QVariant getVariant() = 0;
	virtual void setVariant(const QVariant& variant) = 0;
};



class OptionBool : public Option
{
	Q_OBJECT
	
	friend ProgramOptions;
	
	bool value;
	bool defaultValue;

	QAction action;

	OptionBool(const bool v, const QString& name) : Option(name), value(v), defaultValue(v), action(this)
	{
		action.setText(QObject::tr(name.toStdString().c_str()));
		action.setCheckable(true);
		action.setChecked(v);
		connect(&action, &QAction::triggered      , this   , &OptionBool::setValue);
		connect(this   , &OptionBool::valueChanged, &action, &QAction::setChecked );
	}
	OptionBool(const OptionBool&) = delete;
	OptionBool& operator=(const OptionBool&) = delete;
	
	void setValuePrivat(bool v)
	{
		if(v!=value)
		{
			value = v;
			emit(valueChanged(v));
			if(v)
				emit(trueSignal());
			else
				emit(falseSignal());
		}
	}
	
public:
	virtual void resetValue()                                       { setValuePrivat(defaultValue); };

	
	bool getValue()   const                                         { return value; }
	bool operator()() const                                         { return value; }

	QAction* getAction()                                            { return &action; }
	
	virtual QVariant getVariant()                                   { return QVariant(value); }
	virtual void setVariant(const QVariant& variant)                { setValuePrivat(variant.toBool()); }
	
public slots:
	void setValue(bool v)                                           { setValuePrivat(v); }
	void setValueInvers(bool v)                                     { setValuePrivat(!v); }
	
signals:
	void valueChanged(bool v);
	void trueSignal();
	void falseSignal();
};

class OptionInt : public Option
{
	Q_OBJECT

	friend ProgramOptions;

	int value;
	int defaultValue;

	OptionInt(const int v, const QString& name) : Option(name), value(v), defaultValue(v) {}
	OptionInt(const OptionInt&) = delete;
	OptionInt& operator=(const OptionInt&) = delete;
public:
	virtual void resetValue() { value = defaultValue; emit(valueChanged(value)); };

	int getValue() const { return value; }
	int operator()() const { return value; }
	
	virtual QVariant getVariant() { return QVariant(value); }
	virtual void setVariant(const QVariant& variant) { value = variant.toInt(); }

public slots:
	void setValue(int v) { if(value!=v) { value = v; emit(valueChanged(v));} }

signals:
	void valueChanged(int v);
};


class OptionDouble : public Option
{
	Q_OBJECT

	friend ProgramOptions;

	double value;
	double defaultValue;

	OptionDouble(const double v, const QString& name) : Option(name), value(v), defaultValue(v) {}
	OptionDouble(const OptionDouble&) = delete;
	OptionDouble& operator=(const OptionDouble&) = delete;
public:
	virtual void resetValue() { value = defaultValue; emit(valueChanged(value)); };

	double getValue() const { return value; }
	double operator()() const { return value; }
	
	virtual QVariant getVariant() { return QVariant(value); }
	virtual void setVariant(const QVariant& variant) { value = variant.toDouble(); }

public slots:
	void setValue(double v) { if(value!=v) { value = v; emit(valueChanged(v));} }

signals:
	void valueChanged(double v);
};

class OptionString : public Option
{
	Q_OBJECT

	friend ProgramOptions;

	QString value;
	QString defaultValue;

	OptionString(const QString& v, const QString& name) : Option(name), value(v), defaultValue(v) {}
	OptionString(const OptionString&) = delete;
	OptionString& operator=(const OptionString&) = delete;
public:
	virtual void resetValue() { value = defaultValue; emit(valueChanged(value)); };

	const QString& getValue() const { return value; }
	const QString& operator()() const { return value; }
	
	virtual QVariant getVariant() { return QVariant(value); }
	virtual void setVariant(const QVariant& variant) { value = variant.toString(); }

public slots:
	void setValue(const QString& v) { value = v; emit(valueChanged(v)); }

	signals:
	void valueChanged(const QString& v);
};


class OptionColor : public Option
{
	Q_OBJECT

	friend ProgramOptions;

	QColor value;
	QColor defaultValue;

	OptionColor(const QColor& v, const QString& name) : Option(name), value(v), defaultValue(v) {}
	OptionColor(const OptionColor&) = delete;
	OptionColor& operator=(const OptionColor&) = delete;
public:
	virtual void resetValue() { value = defaultValue; emit(valueChanged(value)); };

	const QColor& getValue() const { return value; }
	const QColor& operator()() const { return value; }
	
	virtual QVariant getVariant() { return QVariant(value); }
	virtual void setVariant(const QVariant& variant) { value = variant.value<QColor>(); }

public slots:
	void setValue(const QColor& v) { value = v; emit(valueChanged(v)); }
	void showColorDialog();

	signals:
	void valueChanged(const QColor& v);
};



#endif


