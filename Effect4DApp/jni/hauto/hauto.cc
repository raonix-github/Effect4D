#include <string.h>
#include <assert.h>
#include <vector>
#include <memory>

#include "com_raonix_effect4dapp_HA210.h"
#include "thread.h"
#include "sigslot.h"
#include "log.h"

#include "libtta/tta.h"
#include "hauto.h"

tta_t *ctx;

namespace raonix
{

HAuto::HAuto(JavaObjectReference *reference)
{
	reference_ = reference;
	Init();
}

HAuto::~HAuto()
{
	LOGI("HAuto::~HAuto");
	delete hauto_light_;
	delete hauto_io_;
	delete hauto_player_;
	delete hauto_motor_;
}

void HAuto::Init()
{
	ctx = tta_new_rs485("/dev/ttyTCC2", 9600, 'N', 8, 1);
	if(ctx == NULL)
	{
		LOGE("HAtuo::Init() fail to open ttyTCC2");
		return;
	}

	if(tta_connect(ctx) == -1)
	{
		LOGE("HAtuo::init(); fail to connect ttyTCC2");
		return;
	}

	////////////////////////////////////////////////////////////////////////////
	tta_set_error_recovery(ctx, (TTA_ERROR_RECOVERY_LINK
				| TTA_ERROR_RECOVERY_PROTOCOL));
//	tta_set_debug(ctx, 1);
	////////////////////////////////////////////////////////////////////////////

#if 1
	// hauto_light_ = new raonix::TTALight("TTA Light");
	hauto_light_ = new raonix::TTALightDasan("Dasan TTA Light");

	hauto_light_->SignalStateChanged.connect(
			this, &HAuto::OnSignalLightStateChanged);

	hauto_light_->start();
#endif

#if 1
	hauto_player_ = new raonix::TTAPlayer("TTA Player");

	hauto_player_->SignalStateChanged.connect(
			this, &HAuto::OnSignalPlayerStateChanged);

	hauto_player_->start();
#endif

#if 1
	hauto_motor_ = new raonix::TTAMotor("TTA Motor");

	hauto_motor_->SignalStateChanged.connect(
			this, &HAuto::OnSignalMotorStateChanged);

	hauto_motor_->start();
#endif

#if 1
	hauto_io_ = new raonix::TTAIO("TTA IO");

	hauto_io_->SignalStateChanged.connect(
			this, &HAuto::OnSignalIOStateChanged);

	hauto_io_->start();
#endif

}

void HAuto::Destroy()
{
}

// 
// Common
//
int HAuto::DeviceSetNum(unsigned char *num)
{
	if(hauto_light_)
	{
		hauto_light_->SetDeviceNum(1, 0);
	}
	if(hauto_io_)
	{
		hauto_io_->SetDeviceNum(0, 1);
	}
	if(hauto_player_)
	{
		hauto_player_->SetDeviceNum(0, 1);
	}
	if(hauto_motor_)
	{
		hauto_motor_->SetDeviceNum(0, 1);
	}

	return 0;
}

//
// Light
//
int HAuto::LightGetState(int grid, unsigned char *state)
{
	int rc;

	if(hauto_light_)
	{
		rc = hauto_light_->GetState(grid, 0xf);
		if(rc < 0)
		{
			return -1;
		}
		rc = sizeof(light_state_t);
		memcpy(state, &hauto_light_->grp_[grid].newstate, rc);
		return rc;
	}

	return -1;
}

int HAuto::LightGetCharacter(int grid, unsigned char *character)
{
	int rc;

	if(hauto_light_)
	{
		rc = hauto_light_->GetCharacter(grid, 0xf);
		if(rc < 0)
		{
			return -1;
		}

		rc = sizeof(light_config_t);
		memcpy(character, &hauto_light_->grp_[grid].config, rc);
		return rc;
	}

	return -1;
}

int HAuto::LightScan()
{
	if(hauto_light_)
	{
		hauto_light_->Scan();
		return 0;
	}

	return -1;
}

int HAuto::LightSetOne(int grid, int swid, int on, int dim)
{
	if(hauto_light_)
	{
		return hauto_light_->SetOne(grid, swid, on, dim);
	}

	return -1;
}

int HAuto::LightSetAll(int grid, int swid, int on)
{
	if(hauto_light_)
	{
		return hauto_light_->SetAll(grid, swid, on);
	}

	return -1;
}

void HAuto::OnSignalLightStateChanged(int grid, int swid)
{
	light_state_t *state;
	int sz = sizeof(light_state_t);

	CALLBACK_START("handleLightStateChanged", "(III[B)V", reference_);
	if (mid != NULL)
	{
		jbyteArray byteArray;

		byteArray = env->NewByteArray(sz);
		if(byteArray == NULL)
		{
			LOGE("error to NewByteArray");
			return;
		}

		if(grid == 0)
		{
			state = &hauto_light_->nogrp_[swid].newstate;
			env->SetByteArrayRegion(byteArray, 0, sz, (jbyte *)state);
		}
		else
		{
			state = &hauto_light_->grp_[grid].newstate;
			env->SetByteArrayRegion(byteArray, 0, sz, (jbyte *)state);
		}

		env->CallVoidMethod(reference_->handler_object, mid, grid, swid, sz,
				byteArray);
	}
	DETACH_FROM_VM(reference_);
}

//
// IO
//
int HAuto::IOScan()
{
	if(hauto_io_)
	{
		hauto_io_->Scan();
		return 0;
	}

	return -1;
}

int HAuto::IOGetState(int grid, unsigned char *state)
{
	int rc;

	if(hauto_io_)
	{
		rc = hauto_io_->GetState(0x0, grid);
		if(rc < 0)
		{
			return -1;
		}
		rc = sizeof(io_state_t);
		memcpy(state, &hauto_io_->nogrp_[grid].newstate, rc);
		return rc;
	}

	return -1;
}

int HAuto::IOGetCharacter(int grid, unsigned char *character)
{
	int rc;

	if(hauto_io_)
	{
		rc = hauto_io_->GetCharacter(0x0, grid);
		if(rc < 0)
		{
			return -1;
		}

		rc = sizeof(io_config_t);
		memcpy(character, &hauto_io_->nogrp_[grid].config, rc);
		return rc;
	}

	return -1;
}

int HAuto::IOSetRelay(int grid, int swid, int val, int mask)
{
	if(hauto_io_)
	{
		return hauto_io_->SetRelay(0, grid, val, mask);
	}

	return -1;
}


void HAuto::OnSignalIOStateChanged(int grid, int swid)
{
	io_state_t *state;
	int sz = sizeof(io_state_t);

	CALLBACK_START("handleIOStateChanged", "(III[B)V", reference_);
	if (mid != NULL)
	{
		jbyteArray byteArray;

		byteArray = env->NewByteArray(sz);
		if(byteArray == NULL)
		{
			LOGE("Error :  NewByteArray");
			return;
		}

		if(grid == 0)
		{
			state = &hauto_io_->nogrp_[swid].newstate;
			env->SetByteArrayRegion(byteArray, 0, sz, (jbyte *)state);
			env->CallVoidMethod(reference_->handler_object, mid, swid, 1, sz,
					byteArray);
		}
		else
		{
			LOGD("TTAIO: not support group\n");
			return ;
		}

	}
	DETACH_FROM_VM(reference_);
}

//
// Player
//
int HAuto::PlayerScan()
{
	if(hauto_player_)
	{
		hauto_player_->Scan();
		return 0;
	}

	return -1;
}

int HAuto::PlayerGetState(int grid, unsigned char *state)
{
	int rc;

	if(hauto_player_)
	{
		rc = hauto_player_->GetState(0x0, grid);
		if(rc < 0)
		{
			return -1;
		}
		rc = sizeof(io_state_t);
		memcpy(state, &hauto_player_->nogrp_[grid].newstate, rc);
		return rc;
	}

	return -1;
}

int HAuto::PlayerGetCharacter(int grid, unsigned char *character)
{
	int rc;

	if(hauto_player_)
	{
		rc = hauto_player_->GetCharacter(0x0, grid);
		if(rc < 0)
		{
			return -1;
		}

		rc = sizeof(io_config_t);
		memcpy(character, &hauto_player_->nogrp_[grid].config, rc);
		return rc;
	}

	return -1;
}

int HAuto::PlayerSet(int grid, int swid, int cmd, unsigned char *buf, int buflen)
{
	if(hauto_player_)
	{
		return hauto_player_->Set(0, grid, cmd, buf, buflen);
	}

	return -1;
}

void HAuto::OnSignalPlayerStateChanged(int grid, int swid)
{
	player_state_t *state;
	int sz = sizeof(player_state_t);

	CALLBACK_START("handlePlayerStateChanged", "(III[B)V", reference_);
	if (mid != NULL)
	{
		jbyteArray byteArray;

		byteArray = env->NewByteArray(sz);
		if(byteArray == NULL)
		{
			LOGE("Error :  NewByteArray");
			return;
		}

		if(grid == 0)
		{
			state = &hauto_player_->nogrp_[swid].newstate;
			env->SetByteArrayRegion(byteArray, 0, sz, (jbyte *)state);
			env->CallVoidMethod(reference_->handler_object, mid, swid, 1, sz,
					byteArray);
		}
		else
		{
			LOGD("TTAPlayer: not support group\n");
			return ;
		}

	}
	DETACH_FROM_VM(reference_);
}

//
// Motor Controller
//
int HAuto::MotorScan()
{
	if(hauto_player_)
	{
		hauto_player_->Scan();
		return 0;
	}

	return -1;
}

int HAuto::MotorGetState(int grid, unsigned char *state)
{
	int rc;

	if(hauto_player_)
	{
		rc = hauto_player_->GetState(0x0, grid);
		if(rc < 0)
		{
			return -1;
		}
		rc = sizeof(io_state_t);
		memcpy(state, &hauto_player_->nogrp_[grid].newstate, rc);
		return rc;
	}

	return -1;
}

int HAuto::MotorGetCharacter(int grid, unsigned char *character)
{
	int rc;

	if(hauto_player_)
	{
		rc = hauto_player_->GetCharacter(0x0, grid);
		if(rc < 0)
		{
			return -1;
		}

		rc = sizeof(io_config_t);
		memcpy(character, &hauto_player_->nogrp_[grid].config, rc);
		return rc;
	}

	return -1;
}

int HAuto::MotorSet(int grid, int swid, int cmd, unsigned char *buf, int buflen)
{
	if(hauto_player_)
	{
		return hauto_player_->Set(0, grid, cmd, buf, buflen);
	}

	return -1;
}

void HAuto::OnSignalMotorStateChanged(int grid, int swid)
{
	player_state_t *state;
	int sz = sizeof(player_state_t);

	CALLBACK_START("handleMotorStateChanged", "(III[B)V", reference_);
	if (mid != NULL)
	{
		jbyteArray byteArray;

		byteArray = env->NewByteArray(sz);
		if(byteArray == NULL)
		{
			LOGE("Error :  NewByteArray");
			return;
		}

		if(grid == 0)
		{
			state = &hauto_player_->nogrp_[swid].newstate;
			env->SetByteArrayRegion(byteArray, 0, sz, (jbyte *)state);
			env->CallVoidMethod(reference_->handler_object, mid, swid, 1, sz,
					byteArray);
		}
		else
		{
			LOGD("TTAMotor: not support group\n");
			return ;
		}

	}
	DETACH_FROM_VM(reference_);
}
}  // namespace raonix
