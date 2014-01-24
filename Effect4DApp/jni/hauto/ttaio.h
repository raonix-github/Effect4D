#ifndef _TTAIO_H_
#define _TTAIO_H_

#include "ttadevice.h"

namespace raonix
{

typedef	struct
{
	unsigned char version;
	unsigned char relay_bmap;
	unsigned char sensor_bmap;
} io_config_t;

typedef	struct
{
	// State
	unsigned char relay;
	unsigned char sensor;
	unsigned char ad;
} io_state_t;

typedef struct
{
	unsigned char exist;
	unsigned char error;
	unsigned char errcnt;

	io_config_t config;
	io_state_t newstate;
	io_state_t oldstate;
} io_info_t;

class TTAIO :
		public TTADevice,
		public Thread,
		public sigslot::has_slots<>
{
private:
	void run();

	int Set(int grid, int swid, int cmd, unsigned char *buf, int buflen);

	int gr_num_;
	int sw_num_[14];
//	int poll_interval_; // usec

public:
	TTAIO(string name);

	virtual ~TTAIO()
	{
	}

	virtual void Scan();
	virtual void Poll();
	virtual int GetState(int grid, int swid);
	virtual int GetCharacter(int grid, int swid);
	virtual int SetRelay(int grid, int swid, int val, int mask);

	int SetDeviceNum(int grpcnt, int nogrpcnt);

	// signals
	sigslot::signal1<int> SignalHAutoEvent;
	sigslot::signal2<int,int> SignalStateChanged;

	io_info_t nogrp_[15];
	io_info_t grp_[15];

	int poll_interval_; // usec
};

}  // namespace raonix

#endif
