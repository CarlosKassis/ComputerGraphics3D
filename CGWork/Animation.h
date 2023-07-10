#pragma once

#include <list>
#include <chrono>
#include "Globals.h"
#include "Physics.h"
#include "Benchmark.h"

using namespace std::chrono;

enum class ActionType
{
	Translate,
	Rotate,
	Scale,
	NOP
};

class Timestamp
{
private:
	time_point<high_resolution_clock> m_timestamp;

public:

	void SetToNow()
	{
		m_timestamp = high_resolution_clock::now();
	}

	long long TimestampInMilliseconds() const
	{
		return duration_cast<milliseconds>(m_timestamp.time_since_epoch()).count();
	}

	long long operator-(const Timestamp& rhs)
	{
		Timestamp out;
		return (m_timestamp - rhs.m_timestamp).count();
	}
};

struct Action
{
	int gameObjectIndex;
	ActionType actionType;
	Space space;
	int axis;
	double amount;
	double timestamp;

	Action() = default;
	Action(int gameObjectIndex, ActionType actionType, Space space, int axis, double amount, double timestamp = 0.0)
	{
		this->gameObjectIndex = gameObjectIndex;
		this->actionType = actionType;
		this->space = space;
		this->axis = axis;
		this->amount = amount;
		this->timestamp = timestamp;
	}
};

enum class ReplaySpeed
{
	Half,
	Normal,
	Double,
	Quadruple
};

class Animator
{
private:
	bool m_bIsRecoding = false;
	bool m_bIsReplaying = false;
	double m_dFrameTime = 0.0;

	Benchmark m_recordStart;
	Benchmark m_replayStart;

	std::list<Action> m_actionsRecorded;
	std::list<Action> m_actionsToBeReplayed;

public:
	bool Repeat = false;
	ReplaySpeed ReplaySpeed = ReplaySpeed::Normal;

	void AddFrameTime()
	{
		m_dFrameTime += 33.3334;
	}

	void StartRecording()
	{
		m_recordStart.Start();
		m_bIsRecoding = true;
		m_actionsRecorded.clear();
	}

	void StopRecording()
	{
		Action action;
		action.actionType = ActionType::NOP;
		AddActionIfRecording(action);

		m_bIsRecoding = false;
	}

	bool HasRecorded()
	{
		return !m_actionsRecorded .empty();
	}

	bool ReplayFinished()
	{
		return m_actionsToBeReplayed.empty();
	}

	void StartReplaying()
	{
		if (!HasRecorded())
			return;

		m_dFrameTime = 0.0;
		m_bIsReplaying = true;
		m_actionsToBeReplayed = m_actionsRecorded;
	}

	void StopReplaying()
	{
		m_bIsReplaying = false;
	}

	bool IsRecording()
	{
		return m_bIsRecoding;
	}

	bool IsReplaying()
	{
		return m_bIsReplaying;
	}

	void AddActionIfRecording(Action& action)
	{
		if (!m_bIsRecoding)
		{
			return;
		}

		action.timestamp = m_recordStart.ElapsedMilliseconds();
		m_actionsRecorded.push_back(action);
	}

	double GetReplaySpeed()
	{
		switch (ReplaySpeed)
		{
		case ReplaySpeed::Half:
			return 0.5;
		case ReplaySpeed::Normal:
			return 1.0;
		case ReplaySpeed::Double:
			return 2.0;
		case ReplaySpeed::Quadruple:
			return 4.0;
		}

		return 1.0;
	}

	bool PopActionsToReplay(std::vector<Action>& actionsToReplay)
	{
		if (!m_bIsReplaying)
			return false;

		if (m_actionsToBeReplayed.empty())
		{
			return false;
		}

		bool hasActionsToReplay = false;
		bool checkForMoreActions = true;
		double replayTime = m_dFrameTime * GetReplaySpeed();
		while (checkForMoreActions && !m_actionsToBeReplayed.empty())
		{
			checkForMoreActions = false;
			if (replayTime >= m_actionsToBeReplayed.front().timestamp)
			{
				actionsToReplay.push_back(m_actionsToBeReplayed.front());
				m_actionsToBeReplayed.pop_front();
				checkForMoreActions = true;
				hasActionsToReplay = true;
			}
		}

		// Stop replay mode when out of actions

		if (m_actionsToBeReplayed.empty() && !Repeat)
		{
			m_bIsReplaying = false;
		}

		return hasActionsToReplay;
	}
};

