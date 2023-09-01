#pragma once

using namespace std;
using namespace Engine;

class IJob
{
public:
	virtual void Execute() { }
};


class HealJob : public IJob
{
public:
	virtual void Execute() override
	{
		// _target->AddHP(_healValue);
		cout << m_Target << "gives heal" << m_HealValue << endl;
	}

public:
	uint64 m_Target = 0;
	uint32 m_HealValue = 0;
};

using JobRef = shared_ptr<IJob>;

class JobQueue
{
public:
	void Push(JobRef job)
	{
		WRITE_LOCK;
		_jobs.push(job);
	}

	JobRef Pop()
	{
		WRITE_LOCK;
		if (_jobs.empty())
			return nullptr;

		JobRef ret = _jobs.front();
		_jobs.pop();
		return ret;
	}

private:
	USE_LOCK;
	queue<JobRef> _jobs;
};