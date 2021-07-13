
#pragma once

#include "Support/Support.h"
#include <eastl/vector.h>
#include <eastl/map.h>

namespace Oak
{
	/**
	\ingroup gr_code_root_task_executor
	*/

	/**
	\brief TaskExecutor

	This class manages execution of tasks. Each task is a callback, i.e. method
	of class. Task can be ordered by level of execution and can be combined in single
	pool via class TaskExecutor::SingleTaskPool. Severel task pools can be combined into
	one group pool via class TaskExecutor::GroupTaskPool.

	*/

	class CLASS_DECLSPEC TaskExecutor
	{
	public:

		class CLASS_DECLSPEC TaskPool
		{
		public:
			virtual void Execute(float dt) = 0;
		};

		class GroupTaskPool;

		class CLASS_DECLSPEC SingleTaskPool : public TaskPool
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

			bool active = true;
			int changeMark = 0;

			TaskList* FindTaskList(int level);
			static void ExecuteList(TaskList* list, float dt);

		public:

			~SingleTaskPool();

			/**
			\brief Set active state

			\param[in] set Define active state. Inactive pools are not executed.
			*/
			void SetActive(bool set);

			/**
			\brief Execute all tasks in a pool

			\param[in] dt Deltatime since last frame.
			*/
			virtual void Execute(float dt);

			/**
			\brief Adding task in a pool

			\param[in] level Priority level of execution. Lower numbers means earlst execution.
			\param[in] entity Pointer to object which method should be executed
			\param[in] call Pointer to a method of a owner class
			\param[in] freq Frequincy of execution of a method

			*/
			void AddTask(int level, Object* entity, Object::Delegate call, float freq = -1.0f);

			/**
			\brief Delete task in a pool

			\param[in] level Priority level of execution. Lower numbers means earlst execution.
			\param[in] entity Pointer to object which method should be executed
			\param[in] new_pool Move task to a new pool

			*/
			void DelTask(int level, Object* entity, SingleTaskPool* new_pool = nullptr);

			/**
			\brief Delete all task of a object in a pool

			\param[in] entity Pointer to object which method should be executed
			\param[in] new_pool Move task to a new pool

			*/
			void DelAllTasks(Object* entity, SingleTaskPool* new_pool = nullptr);
		};

		class CLASS_DECLSPEC GroupTaskPool : public TaskPool
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

			/**
			\brief Add filter of level executions. Filter is used in a case when needed to call only tasks with particular level if execution.

			\param[in] level Level of execution.
			*/
			void AddFilter(int level);

			/**
			\brief Create new task pool in a group

			\return Pointer to TaskExecutor::SingleTaskPool
			*/
			SingleTaskPool* AddTaskPool(const char* file, int line);

			/**
			\brief Delete pool for a group.

			\param[in] pool Pointer to TaskExecutor::SingleTaskPool
			*/
			void DelTaskPool(SingleTaskPool* pool);

			/**
			\brief Execute all tasks pool pool for a group.

			\param[in] dt Deltatime since last frame.
			*/
			virtual void Execute(float dt);

			/**
			\brief Execute all tasks which particular levele of execution

			\param[in] level Level of execution
			\param[in] dt Deltatime since last frame.
			*/
			void ExecutePool(int level, float dt);
		};

		/**
		\brief Create new task pool

		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		\return Pointer to TaskExecutor::SingleTaskPool
		*/
		SingleTaskPool* CreateSingleTaskPool(const char* file, int line);

		/**
		\brief Create new group pool

		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		\return Pointer to TaskExecutor::GroupTaskPool
		*/
		GroupTaskPool* CreateGroupTaskPool(const char* file, int line);
	};
}