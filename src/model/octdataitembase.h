#ifndef OCTDATAITEMBASE_H
#define OCTDATAITEMBASE_H

#include <QObject>

class OctDataItemBase : public QObject
{
	OctDataItemBase* parentItem = nullptr;

	std::vector<OctDataItemBase*> childVec;

	OctDataItemBase(const OctDataItemBase&) = delete;
	OctDataItemBase& operator=(const OctDataItemBase&) = delete;

public:
	// E2EItemBase();
	OctDataItemBase();
	virtual ~OctDataItemBase();


	virtual int childCount() const                              { return childVec.size(); }
	virtual OctDataItemBase* child(int number);
	virtual const QString& getName() const                      { return name; }
	virtual int indexOf(const OctDataItemBase* item) const;


	virtual OctDataItemBase* parent()                           { return parentItem; }
	virtual int row() const                                     { return parentItem?parentItem->indexOf(this):0; }


	virtual bool canRemoveChild() { return false; }
	virtual bool removeChild(OctDataItemBase* /*item*/)         { return false; }

	virtual bool insertChild(OctDataItemBase* /*item*/, int /*position*/) { return false; }


	virtual void itemDoubleClicked() const                      { }
	virtual void itemClicked()       const                      { }

protected:
	QString name = "undef";
	void addBaseElement(OctDataItemBase* ele)                   { ele->parentItem = this; childVec.push_back(ele); }

};

#endif // OCTDATAITEMBASE_H
