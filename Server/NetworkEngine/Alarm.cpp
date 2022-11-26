#include "pch.h"
#include "Alarm.h"
#include "AlarmManager.h"

bool LessNextTick::operator()(const AlarmPtr& lhs, const AlarmPtr& rhs)
{
    return lhs->_nextTick > rhs->_nextTick;
}

Alarm::Alarm(const string& tag, uint32 periodMs, TaskPtr task, bool periodic)
    :
    _tag(tag),
    _periodMs(periodMs),
    _task(std::move(task)),
    _periodic(periodic),
    _enabled(true)
{
    _nextTick = ::GetTickCount64() + (uint64)_periodMs;
}

Alarm::~Alarm()
{
}

void Alarm::Execute()
{
    if (_enabled == false)
        return;

    (*_task)();

    if (_periodic) {
        _nextTick = ::GetTickCount64() + (uint64)_periodMs;
    }
    else {
        Disable();
    }
}

void Alarm::Disable()
{
    _enabled = false;
    AlarmManager::GetInstance()->RemoveAlarm(_tag);
}

AlarmPtr Alarm::Create(const string& tag, uint32 periodMs, TaskPtr task, bool periodic)
{
    return shared_ptr<Alarm>(pool_new<Alarm>(tag, periodMs, std::move(task), periodic), pool_delete<Alarm>);
}

void RegisterAlarm(string tag, uint32 periodMs, TaskPtr task, bool periodic)
{
    AlarmManager::GetInstance()->Register(Alarm::Create(std::move(tag), periodMs, std::move(task), periodic));
}

void RemoveAlarm(string tag)
{
    AlarmManager::GetInstance()->RemoveAlarm(tag);
}
