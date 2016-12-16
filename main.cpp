#include "ThreadPool.h"

class MyTask:public Task
{
				public:
								MyTask(const string &task_name="", void *pri_data=NULL):Task(task_name, pri_data)
								{}
								void run()
								{
												cout << "run my task " << m_task_name << endl;
								}
};
int main()
{
				ThreadPool *pool = new ThreadPool(100);
				sleep(2);
				while(1)
				{
								for(int i=0; i<1000; i++)	
								{
												cout << "index is " << i << endl;
												Task *task = new MyTask("qixianghui", NULL);
												pool->AddTask(task);
								}
				}
				sleep(1000);
}
