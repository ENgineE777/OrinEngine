
#pragma once

#include "Support/Support.h"
#include <eastl/vector.h>
#include <eastl/map.h>

namespace Oak
{
	class TaskExecutor
	{
	public:

		class TaskPool
		{
		public:
			virtual void Execute(float dt) = 0;
		};

		class GroupTaskPool;

		class SingleTaskPool : public TaskPool
		{
		public:

			friend class GroupTaskPool;

			struct Task : DelegateObject
			{
				float freq;
				float time;
			};

			struct TaskList
			{
				int  level;
				eastl::vector<Task> list;
			};

		private:

			eastl::vector<TaskList*> lists;

			bool active;
			int changeMark;

			TaskList* FindTaskList(int level);
			static void ExecuteList(TaskList* list, float dt);

		public:

			SingleTaskPool();

			void SetActive(bool set);
			virtual void Execute(float dt);
			void AddTask(int level, Object* entity, Object::Delegate call, float freq = -1.0f);
			void DelTask(int level, Object* entity, SingleTaskPool* new_pool = nullptr);
			void DelAllTasks(Object* entity, SingleTaskPool* new_pool = nullptr);
		};

		class GroupTaskPool : public TaskPool
		{
			friend class SingleTaskPool;

			struct TaskList
			{
				SingleTaskPool* pool;
				SingleTaskPool::TaskList* list;
			};

			struct GroupList
			{
				int level;
				eastl::vector<TaskList> taskLists;
			};

			eastl::vector<GroupList> groupLists;
			eastl::vector<SingleTaskPool*> taskPools;
			eastl::vector<int> changeMarks;
			eastl::vector<int> filter;

			void FillList();
			void Execute(GroupList& groupList, float dt);

		public:

			void AddFilter(int level);

			SingleTaskPool* AddTaskPool();

			void DelTaskPool(SingleTaskPool* pool);

			virtual void Execute(float dt);

			void ExecutePool(int level, float dt);
		};

		SingleTaskPool* CreateSingleTaskPool();

		GroupTaskPool* CreateGroupTaskPool();
	};
}