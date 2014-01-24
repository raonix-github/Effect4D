#ifndef _TTA_LIGHT_H_
#define _TTA_LIGHT_H_

#include "ttadevice.h"

namespace raonix
{

typedef struct
{
	unsigned char version;
	unsigned char dim_num;
	unsigned char onoff_num;
	unsigned int  type_bmap;
} light_config_t;

typedef struct
{
	unsigned char control[16];
} light_state_t;

typedef struct
{
	unsigned char exist;
	unsigned char error;
	unsigned char errcnt;

	light_config_t config;
	light_state_t newstate;
	light_state_t oldstate;
} light_info_t;

class TTALight :
		public TTADevice,
		public Thread,
		public sigslot::has_slots<>
{
private:
	void run();

	int gr_num_;
	int sw_num_[15];
	int poll_interval_; // usec

public:
	TTALight(string name);

	virtual ~TTALight()
	{
	}

	/* Group ID : 0     : NO Group
	 *          : 1 ~ E : Group ID
	 *          : F     : All Group
	 * Light ID : 0     : Not used
	 *          : 1 ~ E : Light ID
	 *          : F     : All Light
	 */
	int Set(int grid, int swid, int cmd, unsigned char *buf, int buflen);

	virtual void Scan();
	virtual void Poll();
	virtual int GetCharacter(int grid, int swid);
	virtual int GetState(int grid, int swid);
	virtual int SetOne(int grid, int swid, int on, int dim);
	virtual int SetAll(int grid, int swid, int on);

	// signals
	sigslot::signal1<int> SignalHAutoEvent;
	sigslot::signal2<int, int> SignalStateChanged;

	light_info_t nogrp_[15];
	light_info_t grp_[15];
};

}  // namespace raonix

#endif
