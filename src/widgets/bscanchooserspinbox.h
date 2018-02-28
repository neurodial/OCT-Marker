#ifndef BSCANCHOOSERSPINBOX_H
#define BSCANCHOOSERSPINBOX_H

#include<QSpinBox>

class BScanChooserSpinBox : public QSpinBox
{
	Q_OBJECT
public:
	BScanChooserSpinBox(QWidget* parent);

private slots:
	void configBscanChooser();

	void chooserChanged(int value);
	void bscanChanged(int value);
};

#endif // BSCANCHOOSERSPINBOX_H
