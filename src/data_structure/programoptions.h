#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include <QObject>
#include <QVariant>
#include <QColor>
#include <sys/stat.h>
#include <QAction>

#include <vector>

class ProgramOptions;

class Option : public QObject
{
	Q_OBJECT
	
	QString name;
public:
	Option(const QString& name) : name(name) {} // { qDebug("%s",name.toStdString().c_str()); }
	virtual ~Option() {};
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
public:
	virtual void resetValue() { value = defaultValue; emit(valueChanged(value)); };
	
	bool getValue() const { return value; }	
	bool operator()() const { return value; }

	QAction* getAction()                                         { return &action; }
	
	virtual QVariant getVariant() { return QVariant(value); }
	virtual void setVariant(const QVariant& variant) { value = variant.toBool(); action.setChecked(value); }
	
public slots:
	void setValue(bool v) { if(value != v) { value = v; emit(valueChanged(v));} }
	
signals:
	void valueChanged(bool v);
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


class ProgramOptions
{
	ProgramOptions() = delete;
public:
	static OptionBool bscansShowSegmentationslines;
	
	static OptionBool fillEmptyPixelWhite;
	static OptionBool registerBScans;

	static OptionInt  e2eGrayTransform;
	
	static OptionBool sloShowBscans;
	static OptionBool sloShowLabels;
	
	static OptionString octDirectory;
	static OptionString loadOctdataAtStart;
	
	
	static std::vector<Option*> getAllOptions();
	
	static void readAllOptions();
	static void writeAllOptions();
	static void resetAllOptions();
	
	
	// static OptionString cacheDir;
};


#endif // PROGRAMOPTIONS_H
