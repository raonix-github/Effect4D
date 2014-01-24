#ifndef _TTALIGHT_DASAN_H_
#define _TTALIGHT_DASAN_H_

#include "ttalight.h"

namespace raonix
{
class TTALightDasan :
		public TTALight
{
public:
	TTALightDasan(string name);

	virtual ~TTALightDasan()
	{
	}

	virtual int SetOne(int grid, int swid, int on, int dim);

	int SetDeviceNum(int grpcnt, int nogrpcnt);
};

}  // namespace raonix

#endif
