#ifndef _TTAMOTOR_H_
#define _TTAMOTOR_H_

#include "ttadevice.h"

namespace raonix
{

typedef	struct
{
	unsigned char version;
} motor_config_t;

typedef	struct
{
	// State
	unsigned char state;
} motor_state_t;

typedef struct
{
	unsigned char exist;
	unsigned char error;
	unsigned char errcnt;

	motor_config_t config;
	motor_state_t newstate;
	motor_state_t oldstate;
} motor_info_t;

class TTAMotor :
		public TTADevice,
		public Thread,
		public sigslot::has_slots<>
{
private:
	void run();

	int gr_num_;
	int sw_num_[14];
//	int poll_interval_; // usec

public:
	TTAMotor(string name);

	virtual ~TTAMotor()
	{
	}

	virtual void Scan();
	virtual void Poll();
	virtual int GetState(int grid, int swid);
	virtual int GetCharacter(int grid, int swid);

	int SetDeviceNum(int grpcnt, int nogrpcnt);
	int Set (int grid, int swid, int cmd,
		unsigned char *buf, int buflen);
 
	// signals
	sigslot::signal1<int> SignalHAutoEvent;
	sigslot::signal2<int,int> SignalStateChanged;

	motor_info_t nogrp_[15];
	motor_info_t grp_[15];

	int poll_interval_; // usec
};

}  // namespace raonix

#endif
