#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <jni.h>
#include <string>

#include "sigslot.h"
#include "helpers.h"

namespace raonix
{

class System: public sigslot::has_slots<>
{
public:

	enum State
	{
		INIT = 0,
		CHANGED_TEMPERATURE,  // temperature changed
		DE_INIT,              // session is being destroyed
	};
	// initialization
	explicit System(JavaObjectReference *reference);

	~System();
	void Init();
	void Destroy();

	int EnableHWWatchdog(bool on);
	int RefreshHWWatchdog();

private:
	JavaObjectReference *reference_;
};

}  // namespace raonix
#endif  // _SENSOR_H_
