#include <string>
#include <map>

#include "log.h"
#include "thread.h"
#include "sigslot.h"

#include "libtta/tta_rs485.h"
#include "libtta/tta.h"

#include "hauto.h"
#include "ttaio.h"

#define CMD_TYPE_REQ_STATE			0x01
#define CMD_TYPE_REQ_CHAR			0x0f
#define CMD_TYPE_REQ_SETONE			0x41
#define CMD_TYPE_REQ_SETALL			0x42

#define CMD_TYPE_RSP_STATE			0x81
#define CMD_TYPE_RSP_CHAR			0x8f
#define CMD_TYPE_RSP_SETONE			0xC1

extern tta_t *ctx;

namespace raonix
{

TTAIO::TTAIO(string name)
	: poll_interval_(2000000),
	  TTADevice(0x71, name)
{
	io_info_t *grp;
	io_config_t *conf;
	io_state_t *state;
	int i;

	name_ = name;
	dev_id_ = 0x71;

	for(i=0; i<15; i++)
	{
		grp = &nogrp_[i];
		memset(grp, 0, sizeof(io_info_t));

		state = &nogrp_[i].newstate;
		memset(state, 0xff, sizeof(io_state_t));

		conf->version = 1;

		grp = &grp_[i];
		memset(grp, 0, sizeof(io_info_t));

		state = &grp_[i].newstate;
		memset(state, 0xff, sizeof(io_state_t));

		conf->version = 1;
	}
}

void TTAIO::run()
{
	///////////////////////////////
	// Scan();

	try
	{
		while(1)
		{
			Poll();
			usleep(poll_interval_);
		}
	}
	catch (InterruptedException & e)
	{
		LOGE("TTAIO");
	}
}

void TTAIO::Poll()
{
	io_info_t *grp;
	io_state_t *newstate, *oldstate;
	int i;
	int rc;

	// No-group
	for(i=1; i<15; i++)
	{
		grp = &nogrp_[i];
		if(grp->exist)
		{
			rc = GetState(0, i);
			if(rc < 0)
			{
				if(grp->errcnt < MAX_POLLING_ERROR)
				{
					grp->errcnt++;
				}
				else
				{
					grp->exist = 0;
					grp->errcnt = 0;
				}
				continue;
			}
			else
			{
				grp->errcnt = 0;
			}


			newstate = &grp->newstate;
			oldstate = &grp->oldstate;

			// don't care temperature
			// if(memcmp(newstate, oldstate, sizeof(io_state_t)))
			if((newstate->relay != oldstate->relay)
			 || (newstate->sensor != oldstate->sensor))
			{
				SignalStateChanged(0, i);;
			}

			memcpy(oldstate, newstate, sizeof(io_state_t));
		}
	}

	// Group
#if 0
	for(i=1; i<15; i++)
	{
		grp = &grp_[i];
		if(grp->exist)
		{
			rc = GetState(i, 0xf);
			if(rc < 0)
			{
				if(grp->errcnt < MAX_POLLING_ERROR)
				{
					grp->errcnt++;
				}
				else
				{
					grp->exist = 0;
					grp->errcnt = 0;
				}
				continue;
			}
			else
			{
				grp->errcnt = 0;
			}


			newstate = &grp->newstate;
			oldstate = &grp->oldstate;
			// don't care temperature
			// if(memcmp(newstate, oldstate, sizeof(io_state_t)))
			if(newstate->relay != oldstate->relay
			 || newstate->sensor != oldstate->sensor)
			{
				SignalStateChanged(i, 0xf);
			}

			memcpy(oldstate, newstate, sizeof(io_state_t));
		}
	}
#endif
}

void TTAIO::Scan()
{
	int i;

	for(i=1; i<15; i++)
	{
		// No-group
		GetCharacter(0, i);
	}

	LOGD("Scan IO ============");
	LOGD("No Group =================");
	for(i=1; i<15; i++)
	{
		if(nogrp_[i].exist)
		{
			LOGD("IO id   : %d", i);
		}
	}
}

int TTAIO::GetState(int grid, int swid)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	io_info_t *grp;
	io_state_t *state;
	unsigned char subid;
	unsigned char error;
	int i;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		LOGE("IO: not support group");
		return -1;
	}

	subid = (grid << 4) | swid;
	ttareq = ttamsg_make(dev_id_, subid, CMD_TYPE_REQ_STATE, NULL, 0);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		LOGE("IO: fail to get state");
		goto ret;
	}

	// Check response header
	hd = ttarsp->hd;
	if(((hd->subid >> 4) != grid) || ((hd->subid & 0xf) != swid))
	{
		LOGE("IO: invalid data");
		rc = -1;
		goto ret;
	}

	error = ttarsp->data[0];
	// TODO : handle error

	state = &grp->newstate;
	state->relay = ttarsp->data[1];
	state->sensor = ttarsp->data[2];
	state->ad = ttarsp->data[3];

ret:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTAIO::GetCharacter(int grid, int swid)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	io_info_t *grp;
	io_config_t *conf;
	unsigned char subid;
	unsigned char error;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		LOGE("IO: not support group");
		return -1;
	}

	subid = (grid << 4) | swid;
	ttareq = ttamsg_make(dev_id_, subid, CMD_TYPE_REQ_CHAR, NULL, 0);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		grp->exist = 0;
		LOGE("IO: fail to get character");
		goto out;
	}

	hd = ttarsp->hd;
	if(((hd->subid >> 4) != grid) || ((hd->subid & 0xf) != swid))
	{
		LOGE("IO: invalid data");
		grp->exist = 0;
		rc = -1;
		goto out;
	}

	error = ttarsp->data[0];
	// TODO : handle error

	grp->exist = 1;

	conf = &grp->config;
	conf->version = 1;
	conf->relay_bmap = ttarsp->data[1];
	conf->sensor_bmap = ttarsp->data[2];

out:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTAIO::SetRelay(int grid, int swid, int val, int mask)
{
	unsigned char buf[2];

	buf[0] = val;
	buf[1] = mask;
	return Set(grid, swid, CMD_TYPE_REQ_SETONE, buf, sizeof(buf));
}

int TTAIO::Set(int grid, int swid, int cmd,
		unsigned char *buf, int buflen)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	io_info_t *grp;
	io_state_t *state;
	unsigned char subid;
	unsigned char error;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		LOGE("IO: not support group");
		return -1;
	}

	subid = (grid << 4) | swid;
	if(swid == 0xf)
	{
		for(int i=0; i<3; i++)
		{
			ttareq = ttamsg_make(dev_id_, subid, cmd, buf, buflen);
			rc = TTARequest(ctx, ttareq, &ttarsp);
			if(rc < 0)
			{
				LOGE("IO : fail to set. retry %d times", i);
				continue;
			}
			else
			{
				goto ret;
			}
		}
	}
	else
	{
		ttareq = ttamsg_make(dev_id_, subid, cmd, buf, buflen);
		rc = TTARequest(ctx, ttareq, &ttarsp);
		if(rc < 0)
		{
			LOGE("IO : fail to set");
			goto ret;
		}

		hd = ttarsp->hd;
		grid = hd->subid >> 4;	// Group ID
		swid = hd->subid & 0xf;	// Switch ID

		error = ttarsp->data[0];
		// TODO : handle error

		state = &grp->newstate;
		state->relay = ttarsp->data[1];
		state->sensor = ttarsp->data[2];
	}

ret:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTAIO::SetDeviceNum(int grpcnt, int nogrpcnt)
{
	if(nogrpcnt >= 0 && nogrpcnt < 15)
	{
		for(int i=1; i<=nogrpcnt; i++)
		{
			nogrp_[i].exist = 1;
		}

		for(int i=nogrpcnt+1; i<15; i++)
		{
			nogrp_[i].exist = 0;
		}
	}

	return 0;
}

}  // namespace raonix
