#include "Singleton.h"
#include "ITaskCreator.h"
#include "Alarm.h"

static constexpr uint32 LongPeriodAlarmMs = 3000;

class TaskSchedule;
class AlarmManager : public ISingleton<AlarmManager>
{
public:
	AlarmManager();
	~AlarmManager();
	
	void Register(AlarmPtr alaram);

private:
	class AlarmScheduler : public ITaskCreator
	{
		friend class AlaramManager;
		using AlarmQueue = priority_queue<AlarmPtr, deque<AlarmPtr>, LessNextTick>;
	private:
		atomic<bool>		   _pending;
		TaskSchedule*		   _check_schedule;
		function<void(TaskSchedule*)> _schedule_func;

		StdMutex  _mtx;
		AlarmQueue _alarm_que;
	public:
		AlarmScheduler(function<void(TaskSchedule*)> schedule_func);
		~AlarmScheduler();

		void Push(AlarmPtr alarm);
		void onCheckSchedule(AlarmQueue& alarmQueue);
		void ReSchedule(AlarmPtr alarm);

		virtual void Submit() override;
		virtual void EndSchedule() override;
	};

	shared_ptr<AlarmScheduler> _longtermScheduler;
	shared_ptr<AlarmScheduler> _shorttermScheduler;

public:
	StdMutex			  _mtx;
	map<string, AlarmPtr> _alarms;

	bool AddAlarm(AlarmPtr alarm);
	void RemoveAlarm(const string& tag);
};
