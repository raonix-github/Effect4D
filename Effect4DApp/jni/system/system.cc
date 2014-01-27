#include <string.h>
#include <assert.h>
#include <vector>
#include <memory>

#include "com_raonix_effect4dapp_HA210.h"
#include "thread.h"
#include "sigslot.h"
#include "log.h"

#include "system/system.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int system_camera_change(int ch);
extern int system_enable_hwwatchdog(int on);
extern int system_refresh_hwwatchdog();

#ifdef __cplusplus
}
#endif

namespace raonix
{

System::System(JavaObjectReference *reference)
{
	reference_ = reference;
	Init();
}

System::~System()
{
	LOGI("System::~System");
}

void System::Init()
{
}

void System::Destroy()
{
}

int System::ChangeCamera(int ch)
{
	LOGD("[TRC %s:%d]", __func__, __LINE__);
	return system_camera_change(ch);
}

int System::EnableHWWatchdog(bool on)
{
	return system_enable_hwwatchdog(on);
}

int System::RefreshHWWatchdog()
{
	return system_refresh_hwwatchdog();
}

}  // namespace raonix
