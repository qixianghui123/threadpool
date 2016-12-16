#include "ThreadPool.h"
#include <algorithm>

vector<Task*> ThreadPool::m_task_v;
pthread_mutex_t ThreadPool::m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::m_cond = PTHREAD_COND_INITIALIZER;


ThreadPool::ThreadPool(int thread_num)
{
				pthread_mutex_init(&m_idle_mutex, NULL);
				pthread_mutex_init(&m_busy_mutex, NULL);
				m_thread_num = thread_num;
				Create();
}

ThreadPool::~ThreadPool()
{}

void ThreadPool::Create()
{
				m_tid = new pthread_t[m_thread_num];
				for(int i=0; i<m_thread_num; i++)
				{
								pthread_create(&m_tid[i], NULL, routine, (void *)this);
				}
}

void ThreadPool::AddTask(Task *task)
{
				pthread_mutex_lock(&m_mutex);
				m_task_v.push_back(task);
				pthread_cond_signal(&m_cond);
				pthread_mutex_unlock(&m_mutex);
				MoveToBusyList();
}

void* ThreadPool::routine(void *s)
{
				while(1)
				{
								pthread_mutex_lock(&m_mutex);
								while(m_task_v.size()<=0)
								{
												pthread_cond_wait(&m_cond, &m_mutex);
								}
								vector<Task*>::iterator it = m_task_v.begin();
								Task *task = *it;
								if(it != m_task_v.end())
								{
												task = *it;
												m_task_v.erase(it);
								}
								pthread_mutex_unlock(&m_mutex);
								task->run();
								delete task;
								cout << "delete task success" << endl;
								ThreadPool *pool = (ThreadPool*)s;
								if(pool)
								{
												pool->MoveToIdleList();
								}
				}
}

void ThreadPool::AddToIdleList(pthread_t tid)
{
				pthread_mutex_lock(&m_idle_mutex);
				if(m_idle_list.size() < m_thread_num)
				{
								m_idle_list.push_back(tid);
				}
				pthread_mutex_unlock(&m_idle_mutex);
}

void ThreadPool::MoveToIdleList()
{
				while(m_idle_list.size()>=m_thread_num)
				{
								usleep(10);
				}

				while(m_busy_list.size()<=0)
				{
								usleep(10);
				}

				pthread_mutex_lock(&m_busy_mutex);
				pthread_t tid = m_busy_list.front();
				vector<pthread_t>::iterator it = find(m_busy_list.begin(), m_busy_list.end(), tid);
				m_busy_list.erase(it);
				pthread_mutex_unlock(&m_busy_mutex);

				pthread_mutex_lock(&m_idle_mutex);
				m_idle_list.push_back(tid);
				pthread_mutex_unlock(&m_idle_mutex);
}

void ThreadPool::MoveToBusyList()
{
				while(m_busy_list.size()>=m_thread_num)
				{
								        usleep(10);
				}

				while(m_idle_list.size()<=0)
				{
								        usleep(10);
				}

				pthread_mutex_lock(&m_idle_mutex);
				pthread_t tid = m_idle_list.front();
				vector<pthread_t>::iterator it = find(m_idle_list.begin(), m_idle_list.end(), tid);
				m_idle_list.erase(it);
				pthread_mutex_unlock(&m_idle_mutex);

				pthread_mutex_lock(&m_busy_mutex);
				m_busy_list.push_back(tid);
				pthread_mutex_unlock(&m_busy_mutex);
}



