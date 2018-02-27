#pragma once


class MarkerCommand
{
public:
	virtual ~MarkerCommand() {}

	virtual void redo()  = 0;
	virtual void undo()  = 0;
	virtual void apply() = 0;
	int getBScan() const { return bscan; }

protected:
	int bscan = -1;
};
