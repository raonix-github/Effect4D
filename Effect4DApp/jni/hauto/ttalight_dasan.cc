#include <string>
#include <map>

#include "log.h"
#include "thread.h"
#include "sigslot.h"

#include "libtta/tta_rs485.h"
#include "libtta/tta.h"
#include "ttalight_dasan.h"

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

TTALightDasan::TTALightDasan(string name)
	: TTALight(name)
{
}

int TTALightDasan::SetOne (int grid, int swid, int on, int dim)
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
//		buf[1] = ((dim & 0xf) << 4) | on;
		buf[1] = on;
		buf[2] = 0x00;
		buflen = 3;
	}
	else
	{
		buf[0] = (dim << 4) | (on & 0x1);
		buflen = 1;
	}

	return Set(grid, 1, CMD_TYPE_REQ_ONECTRL, buf, buflen);
}

int TTALightDasan::SetDeviceNum(int grpcnt, int nogrpcnt)
{
	if(grpcnt >= 0 && grpcnt < 15)
	{
		for(int i=1; i<=grpcnt; i++)
		{
			grp_[i].exist = 1;
			grp_[i].config.version = 1;
			grp_[i].config.type_bmap = 0x80000000; // Not supported
		}

		for(int i=grpcnt+1; i<15; i++)
		{
			grp_[i].exist = 0;
			grp_[i].config.version = 1;
			grp_[i].config.type_bmap = 0x80000000; // Not supported
		}
	}

	return 0;
}

}  // namespace raonix
