#ifndef WGOCTDATATREE_H
#define WGOCTDATATREE_H

#include <QWidget>

class QListView;
class OctDataModel;
class OctFilesModel;

class WGOctDataTree : public QWidget
{
	Q_OBJECT

	QListView*  listviewFiles = nullptr;
	OctFilesModel* modelFiles = nullptr;
	
	QListView* listviewOctData = nullptr;
	OctDataModel* modelOctData = nullptr;

public:
	WGOctDataTree();


};

#endif // WGOCTDATATREE_H
