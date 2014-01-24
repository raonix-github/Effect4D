#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <string>
#include <stdexcept>

using namespace std;

class Thread
{
	public:
		Thread();
		Thread(const std::string &name);
		virtual ~Thread();
		void start();
		void join();
		void join(unsigned long time);
		void interrupt();
		bool isInterrupted();
		bool isAlive();
		const std::string& getName();
		void setName(const std::string& name);

	private:
		int _state;
		bool _joinning;
		bool _isInterrupted;
		pthread_mutex_t _mutex;
		pthread_cond_t _cond;
		static pthread_attr_t _attr;
		pthread_t _thread;
		static long _id;
		std::string _name;
		static void* threadHandler(void* arg);
		static void interruptHandler(int sigInt);
		void init(const std::string& name);
		virtual void run() = 0;
};

class InterruptedException : public exception 
{
	public:
		explicit InterruptedException(const std::string&  message)
			: _message(message)
		{}

		virtual ~InterruptedException() throw()
		{}

		virtual const char* what() const throw()
		{
			return _message.c_str();
		}

	private:
		std::string _message;

};


class IllegalThreadStateException : public exception 
{
	public:
		explicit IllegalThreadStateException(const std::string&  message)
			: _message(message)
		{}

		virtual ~IllegalThreadStateException() throw()
		{}

		virtual const char* what() const throw()
		{
			return _message.c_str();
		}

	private:
		std::string _message;

};

#endif /*THREAD_H_*/
