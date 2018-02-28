#pragma once


class MarkerCommand
{
public:
	virtual ~MarkerCommand() {}

	virtual bool redo()  = 0;
	virtual bool undo()  = 0;
	virtual void apply() = 0;
	int getBScan() const { return bscan; }

protected:
	int bscan = -1;
};
