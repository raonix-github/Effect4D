#include <string>
#include <map>

#include "log.h"
#include "thread.h"
#include "sigslot.h"

#include "libtta/tta_rs485.h"
#include "libtta/tta.h"

#include "hauto.h"
#include "ttaplayer.h"

#define CMD_TYPE_REQ_STATE			0x01
#define CMD_TYPE_REQ_CHAR			0x0f

#define CMD_TYPE_RSP_STATE			0x81
#define CMD_TYPE_RSP_CHAR			0x8f

extern tta_t *ctx;

namespace raonix
{

TTAPlayer::TTAPlayer(string name)
	: poll_interval_(2000000),
	  TTADevice(0x80, name)
{
	player_info_t *grp;
	player_config_t *conf;
	player_state_t *state;
	int i;

	name_ = name;
	dev_id_ = 0x80;

	for(i=0; i<15; i++)
	{
		grp = &nogrp_[i];
		memset(grp, 0, sizeof(player_info_t));

		state = &nogrp_[i].newstate;
		memset(state, 0xff, sizeof(player_state_t));

		conf->version = 1;
	}
}

void TTAPlayer::run()
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
		LOGE("TTAPlayer");
	}
}

void TTAPlayer::Poll()
{
	player_info_t *grp;
	player_state_t *newstate, *oldstate;
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
			// if(memcmp(newstate, oldstate, sizeof(player_state_t)))
			if(newstate->state != oldstate->state)
			{
				SignalStateChanged(0, i);;
			}

			memcpy(oldstate, newstate, sizeof(player_state_t));
		}
	}
}

void TTAPlayer::Scan()
{
	int i;

	for(i=1; i<15; i++)
	{
		// No-group
		GetCharacter(0, i);
	}

	LOGD("Scan Player ============");
	LOGD("No Group =================");
	for(i=1; i<15; i++)
	{
		if(nogrp_[i].exist)
		{
			LOGD("Player id   : %d", i);
		}
	}
}

int TTAPlayer::GetState(int grid, int swid)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	player_info_t *grp;
	player_state_t *state;
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
		LOGE("Player: not support group");
		return -1;
	}

	subid = (grid << 4) | swid;
	ttareq = ttamsg_make(dev_id_, subid, CMD_TYPE_REQ_STATE, NULL, 0);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		LOGE("Player: fail to get state");
		goto ret;
	}

	// Check response header
	hd = ttarsp->hd;
	if(((hd->subid >> 4) != grid) || ((hd->subid & 0xf) != swid))
	{
		LOGE("Player: invalid data");
		rc = -1;
		goto ret;
	}

	error = ttarsp->data[0];
	// TODO : handle error

	state = &grp->newstate;
	state->state = ttarsp->data[1];

ret:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTAPlayer::GetCharacter(int grid, int swid)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	player_info_t *grp;
	player_config_t *conf;
	unsigned char subid;
	unsigned char error;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		LOGE("Player: not support group");
		return -1;
	}

	subid = (grid << 4) | swid;
	ttareq = ttamsg_make(dev_id_, subid, CMD_TYPE_REQ_CHAR, NULL, 0);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		grp->exist = 0;
		LOGE("Player: fail to get character");
		goto out;
	}

	hd = ttarsp->hd;
	if(((hd->subid >> 4) != grid) || ((hd->subid & 0xf) != swid))
	{
		LOGE("Player: invalid data");
		grp->exist = 0;
		rc = -1;
		goto out;
	}

	error = ttarsp->data[0];
	// TODO : handle error

	grp->exist = 1;

	conf = &grp->config;
	conf->version = 1;

out:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTAPlayer::GetControlData(int grid, int swid, int offset,
		unsigned char *buf)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	player_info_t *grp;
	unsigned char subid;
	unsigned char error;
	unsigned char pl[3];
	int i;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		LOGE("Player: not support group");
		return -1;
	}

	subid = (grid << 4) | swid;
	pl[0] = 0x02;	// upload
	pl[1] = (offset >> 8) & 0xff;
	pl[2] = offset & 0xff;
	ttareq = ttamsg_make(dev_id_, subid, 0x41, pl, 3);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		LOGE("Player: fail to get state");
		goto ret;
	}

	// Check response header
	hd = ttarsp->hd;
	if(((hd->subid >> 4) != grid) || ((hd->subid & 0xf) != swid))
	{
		LOGE("Player: invalid data");
		rc = -1;
		goto ret;
	}

	error = ttarsp->data[0];
	// TODO : handle error

	rc = hd->datalen;
	for(i=1; i<rc; i++)
	{
		buf[i-1] = ttarsp->data[i];
	}

ret:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTAPlayer::SetControlData(int grid, int swid, int offset,
		unsigned char *buf, int buflen)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	player_info_t *grp;
	player_state_t *state;
	unsigned char subid;
	unsigned char error;
	unsigned char *pl = NULL;
	int pllen;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		LOGE("Player: not support group");
		return -1;
	}

	subid = (grid << 4) | swid;
	pl = (unsigned char *)malloc(buflen+3);
	pl[0] = 0x01; 	// download
	pl[1] = (offset >> 8) & 0xff;
	pl[2] = offset & 0xff;
	memcpy(pl+3, buf, buflen);
	pllen = buflen + 3;

	ttareq = ttamsg_make(dev_id_, subid, 0x41, buf, buflen);
	rc = TTARequest(ctx, ttareq, &ttarsp);
	if(rc < 0)
	{
		LOGE("Player: fail to set");
		goto ret;
	}

	hd = ttarsp->hd;
	grid = hd->subid >> 4;	// Group ID
	swid = hd->subid & 0xf;	// Switch ID

	error = ttarsp->data[0];
	// TODO : handle error

	state = &grp->newstate;
	state->state = ttarsp->data[1];

ret:
	if(pl)
		free(pl);

	if(ttarsp)
		ttamsg_free(ttarsp);

	return rc;
}

int TTAPlayer::Set(int grid, int swid, int cmd,
		unsigned char *buf, int buflen)
{
	ttamsg_t *ttareq=NULL, *ttarsp=NULL;
	ttamsg_hd_t *hd;
	player_info_t *grp;
	player_state_t *state;
	unsigned char subid;
	unsigned char error;
	int rc;

	if(grid == 0)
	{
		grp = &nogrp_[swid];
	}
	else
	{
		LOGE("Player: not support group");
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
				LOGE("Player: fail to set. retry %d times", i);
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
			LOGE("Player: fail to set");
			goto ret;
		}

		hd = ttarsp->hd;
		grid = hd->subid >> 4;	// Group ID
		swid = hd->subid & 0xf;	// Switch ID

		error = ttarsp->data[0];
		// TODO : handle error

		state = &grp->newstate;
		state->state = ttarsp->data[1];
	}

ret:
	if(ttarsp)
	{
		ttamsg_free(ttarsp);
	}

	return rc;
}

int TTAPlayer::SetDeviceNum(int grpcnt, int nogrpcnt)
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
