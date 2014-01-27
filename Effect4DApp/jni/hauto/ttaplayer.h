#ifndef _TTAPLAYER_H_
#define _TTAPLAYER_H_

#include "ttadevice.h"

namespace raonix
{

typedef	struct
{
	unsigned char version;
} player_config_t;

typedef	struct
{
	// State
	unsigned char state;
} player_state_t;

typedef struct
{
	unsigned char exist;
	unsigned char error;
	unsigned char errcnt;

	player_config_t config;
	player_state_t newstate;
	player_state_t oldstate;
} player_info_t;

class TTAPlayer :
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
	TTAPlayer(string name);

	virtual ~TTAPlayer()
	{
	}

	virtual void Scan();
	virtual void Poll();
	virtual int GetState(int grid, int swid);
	virtual int GetCharacter(int grid, int swid);

	int SetDeviceNum(int grpcnt, int nogrpcnt);
	int Set (int grid, int swid, int cmd,
		unsigned char *buf, int buflen);
 
	int GetControlData(int grid, int swid, int offset, unsigned char *buf);
	int SetControlData(int grid, int swid, int offset, unsigned char *buf,
			int buflen);

	// signals
	sigslot::signal1<int> SignalHAutoEvent;
	sigslot::signal2<int,int> SignalStateChanged;

	player_info_t nogrp_[15];
	player_info_t grp_[15];

	int poll_interval_; // usec
};

}  // namespace raonix

#endif
