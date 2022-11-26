#pragma once

class ITaskCreator : public enable_shared_from_this<ITaskCreator>
{
public:
	virtual void Submit() abstract;
	virtual void EndSchedule() abstract;
};
