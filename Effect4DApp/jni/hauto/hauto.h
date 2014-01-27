#ifndef _HAUTO_H_
#define _HAUTO_H_

#include <jni.h>
#include <string>
#include <cstring>

#include "sigslot.h"
#include "helpers.h"
#include "ttalight.h"
#include "ttalight_dasan.h"
#include "ttaplayer.h"

namespace raonix
{
// class TTALight;

class HAuto: public sigslot::has_slots<>
{
private:
	void OnSignalHAutoEvent(int event);
	void OnSignalLightStateChanged(int grid, int swid);
	void OnSignalPlayerStateChanged(int grid, int swid);

	JavaObjectReference *reference_;
//	raonix::TTALight *hauto_light_;
	raonix::TTALightDasan *hauto_light_;
	raonix::TTAPlayer *hauto_player_;

public:
	~HAuto();
	void Init();
	void Destroy();

	int DeviceSetNum(unsigned char *num);

	int LightScan();
	int LightGetState(int grid, unsigned char *state);
	int LightGetCharacter(int grid, unsigned char *character);
	int LightSetOne(int grid, int swid, int on, int dim);
	int LightSetAll(int grid, int swid, int on);

	int PlayerScan();
	int PlayerGetState(int grid, unsigned char *state);
	int PlayerGetCharacter(int grid, unsigned char *character);
	int PlayerSet(int grid, int swid, int cmd, unsigned char *buf, int buflen);

	int PlayerGetControlData(int grid, int swid, int offset, unsigned char *buf);
	int PlayerSetControlData(int grid, int swid, int offset, unsigned char *buf,
			int buflen);

	enum Event
	{
		INIT = 0,
		IN_PROGRESS,
		DE_INIT,
		SCAN_DONE,
	};

	// initialization
	explicit HAuto(JavaObjectReference *reference);
};

}  // namespace raonix
#endif  // _HAUTO_H_
