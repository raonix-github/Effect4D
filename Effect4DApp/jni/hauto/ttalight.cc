#include <string>
#include <map>

#include "log.h"
#include "thread.h"
#include "sigslot.h"

#include "libtta/tta_rs485.h"
#include "libtta/tta.h"
#include "ttalight.h"

#define CMD_TYPE_REQ_STATE		0x01
#define CMD_TYPE_REQ_CHAR		0x0f
#define CMD_TYPE_REQ_ONECTRL	0x41
#define CMD_TYPE_REQ_ALLCTRL	0x42
#define CMD_TYPE_RSP_STAT		0x81
#define CMD_TYPE_RSP_CHAR		0x8f
#define CMD_TYPE_RSP_ONECALL	0xc1

extern tta_t *ctx;

namespace raonix
{

TTALight::TTALight(string name)
	: poll_interval_(2000000),
	  TTADevice(0x0E, name)
{
	light_info_t *grp;
	light_state_t *state;
	light_config_t *conf;
	int i;

	for(i=0; i<15; i++)
	{
		grp = &nogrp_[i];
		memset(grp, 0, sizeof(light_info_t));

		state = &nogrp_[i].newstate;
		memset(state, 0xff, sizeof(light_state_t));

		conf->version = 3;

		grp = &grp_[i];
		memset(grp, 0, sizeof(light_info_t));

		state = &grp_[i].newstate;
		memset(state, 0xff, sizeof(light_state_t));

		conf->version = 3;
	}
}

void TTALight::run()
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
		LOGE("TTALight");
	}
}

void TTALight::Poll()
{
	light_info_t *grp;
	light_state_t *newstate, *oldstate;
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
					LOGE("Light: too many polling error");
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

			if(memcmp(newstate, oldstate, sizeof(light_state_t)))
			{
				SignalStateChanged(0, i);;
			}
			memcpy(oldstate, newstate, sizeof(light_state_t));
		}
	}

	// Group
	for(i=1; i<15; i++)
	{
		grp = &grp_[i];
		// if(grp->onoff_num || grp->dim_num)
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
					LOGE("Light: too many polling error");
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
			if(memcmp(newstate, oldstate, sizeof(light_state_t)))
			{
				SignalStateChanged(i, 0xf);
			}
			memcpy(oldstate, newstate, sizeof(light_state_t));
		}
	}
}

void TTALight::Scan()
{
	int i;

	for(i=1; i<15; i++)
	{
		// No-group
		GetCharacter(0, i);

		// Groups
		GetCharacter(i, 0xf);
	}

	LOGD("Scan light ===============\n");
	LOGD("No Group =================\n");
	for(i=1; i<15; i++)
	{
		if(nogrp_[i].exist)
		{
			LOGD("proto ver : %d\n", i);
			LOGD("Switch id : %d\n", i);
			LOGD("onoff_num : %d\n", nogrp_[i].config.onoff_num);
			LOGD("dim_num   : %d\n", nogrp_[i].config.dim_num);
		}
	}

	LOGD("Group    =================\n");
	for(i=1; i<15; i++)
	{
		if(grp_[i].exist)
		{
			LOGD("proto ver : %d\n", i);
			LOGD("group id  : %d\n", i);
			LOGD("onoff_num : %d\n", grp_[i].config.onoff_num);
			LOGD("dim_num   : %d\n", grp_[i].config.dim_num);
		}
	}
}

int TTALight::GetState(int grid, int swid)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	light_info_t *grp;
	light_state_t *state;
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
		grp = &grp_[grid];
	}


	subid = (grid << 4) | swid;
	ttareq = ttamsg_make(dev_id_, subid, CMD_TYPE_REQ_STATE, NULL, 0);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		LOGE("Light(%d:%d): fail to get state", grid, swid);
		goto out;
	}

	// Check response header
	hd = ttarsp->hd;
	if(((hd->subid >> 4) != grid) || ((hd->subid & 0xf) != swid))
	{
		LOGE("Light: invalid data");
		rc = -1;
		goto out;
	}

	error = ttarsp->data[0];
	// TODO : handle error

	state = &grp->newstate;
	if(grid == 0)
	{
		state->control[0] = ttarsp->data[1];
	}
	else
	{
		for(i=1; i<hd->datalen; i++)
		{
			state->control[i-1] = ttarsp->data[i];
		}
	}

out:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTALight::GetCharacter(int grid, int swid)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	light_info_t *grp;
	light_config_t *conf;
	unsigned char subid;
	unsigned char error;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		grp = &grp_[grid];
	}


	subid = (grid << 4) | swid;
	ttareq = ttamsg_make(dev_id_, subid, CMD_TYPE_REQ_CHAR, NULL, 0);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		LOGE("Light: fail to get character");
		goto out;
	}

	hd = ttarsp->hd;
	if(((hd->subid >> 4) != grid) || ((hd->subid & 0xf) != swid))
	{
		LOGE("Thermostat: invalid data");
		rc = -1;
		goto out;
	}

	error = ttarsp->data[0];
	// TODO : handle error

	grp->exist = 1;

	conf = &grp->config;
	conf->onoff_num = ttarsp->data[1];
	conf->dim_num = ttarsp->data[2];
	if(hd->datalen == 5)		// TTAK.KO-04.0073/R3
	{
		conf->version = 3;
		conf->type_bmap = (ttarsp->data[4] << 8) | ttarsp->data[3];
	}
	else
	{
		conf->version = 1;
		conf->type_bmap = 0x80000000; // Not supported
	}

out:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTALight::Set (int grid, int swid, int cmd,
		unsigned char *buf, int buflen)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	light_info_t *grp;
	light_state_t *state;
	unsigned char subid;
	unsigned char error;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		grp = &grp_[grid];
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
				LOGE("Light: fail to set. retry %d times", i);
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
			LOGE("Light: fail to control");
			goto ret;
		}

		hd = ttarsp->hd;
		grid = hd->subid >> 4;	// Group ID
		swid = hd->subid & 0xf;	// Switch ID

		error = ttarsp->data[0];
		// TODO : handle error

		state = &grp->newstate;
		if(grid == 0)
		{
			state->control[0] = ttarsp->data[1];
		}
		else
		{
			state->control[swid] = ttarsp->data[1];
		}
	}

ret:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTALight::SetOne (int grid, int swid, int on, int dim)
{
	light_info_t *grp;
	unsigned char buf[3];
	int buflen;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		grp = &grp_[grid];
	}

	if(grp->config.version == 1)
	{
		buf[0] = swid;
		buf[1] = ((dim & 0xf) << 4) | on;
		buf[2] = 0x00;
		buflen = 3;
	}
	else
	{
		buf[0] = (dim << 4) | (on & 0x1);
		buflen = 1;
	}

	return Set(grid, swid, CMD_TYPE_REQ_ONECTRL, buf, buflen);
}

int TTALight::SetAll(int grid, int swid, int on)
{
	light_info_t *grp;
	unsigned char buf[3];
	int buflen;
	unsigned char subid;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		grp = &grp_[grid];
	}

	subid = (grid << 4) | 0xf;
	if(grp->config.version == 1)
	{
		buf[0] = subid;
		buf[1] = 0x00;
		buf[2] = (on) ? 0xFF : 0x00;
		buflen = 3;
	}
	else
	{
		buf[0] = on & 0x1;
		buflen = 1;
	}

	return Set(grid, swid, CMD_TYPE_REQ_ALLCTRL, buf, buflen);
}

}  // namespace raonix
