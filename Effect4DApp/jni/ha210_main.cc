#include <jni.h>
#include <assert.h>
#include <string>

#include "log.h"
#include "helpers.h"
#include "thread.h"

#include "hauto/ttadevice.h"
#include "hauto/hauto.h"
#include "hauto/ttalight.h"
#include "system/system.h"

#include "com_raonix_effect4dapp_HA210.h"

raonix::System *system_;
raonix::HAuto *hauto_;

jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
	if (!vm)
	{
		LOGE("JNI_OnLoad did not receive a valid VM pointer");
		return JNI_ERR;
	}
	JNIEnv* env;
	if (JNI_OK != vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6))
	{
		LOGE("JNI_OnLoad could not get JNI env");
		return JNI_ERR;
	}

	return JNI_VERSION_1_6;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_raonix_effect4dapp_HA210_nativeInit
	(JNIEnv *env, jobject object, jobject context )
{
	LOGI("Java_com_raonix_effect4dapp_HA210_nativeInit - initializing ");

	JavaObjectReference *instance = NEW_OBJECT(JavaObjectReference, 1);
	RETURN_IF_FAIL(instance != NULL);
	SetJavaObject(instance, env, object, context);

	if (!system_)
	{
		system_ = new raonix::System(instance);
	}

	if (!hauto_)
	{
		hauto_ = new raonix::HAuto(instance);
	}
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeRelease
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_raonix_effect4dapp_HA210_nativeRelease
	(JNIEnv *env, jobject object)
{
	LOGI("Java_com_raonix_effect4dapp_HA210_nativeRelease - Release ");

	if (system_)
	{
		system_->Destroy();
		delete system_;
		system_ = NULL;
	}

	if (hauto_)
	{
		hauto_->Destroy();
		delete hauto_;
		hauto_ = NULL;
	}
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoDeviceSetNum
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoDeviceSetNum
	(JNIEnv *env, jobject object, jbyteArray num)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(num, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->DeviceSetNum((unsigned char *)arr);

		env->ReleaseByteArrayElements(num, arr, 0);
		return rc;
	}
	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoLightScan
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoLightScan
  (JNIEnv *env, jobject object)
{
	if (hauto_)
	{
		return hauto_->LightScan();
	}
	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoLightGetState
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoLightGetState
  (JNIEnv *env, jobject object, jint grid, jbyteArray state)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(state, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->LightGetState(grid, (unsigned char *)arr);

		env->ReleaseByteArrayElements(state, arr, 0);
		return rc;
	}

	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoLightGetCharacter
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoLightGetCharacter
  (JNIEnv *env, jobject object, jint grid, jbyteArray character)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(character, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->LightGetCharacter(grid, (unsigned char *)arr);

		env->ReleaseByteArrayElements(character, arr, 0);
		return rc;
	}

	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoLightSetOne
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoLightSetOne
  (JNIEnv *env, jobject object, jint grid, jint swid, jint on, jint dim)
{
	if (hauto_)
	{
		return hauto_->LightSetOne(grid, swid, on, dim);
	}

	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoLightSetAll
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoLightSetAll
  (JNIEnv *env, jobject object, jint grid, jint swid, jint on)
{
	if (hauto_)
	{
		return hauto_->LightSetAll(grid, swid, on);
	}

	return -1;
}
/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoPlayerScan
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoPlayerScan
  (JNIEnv *env, jobject object)
{
	if (hauto_)
	{
		return hauto_->PlayerScan();
	}
	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoPlayerGetState
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoPlayerGetState
  (JNIEnv *env, jobject object, jint grid, jbyteArray state)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(state, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->PlayerGetState(grid, (unsigned char *)arr);

		env->ReleaseByteArrayElements(state, arr, 0);
		return rc;
	}

	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoPlayerGetCharacter
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoPlayerGetCharacter
  (JNIEnv *env, jobject object, jint grid, jbyteArray character)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(character, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->PlayerGetCharacter(grid, (unsigned char *)arr);

		env->ReleaseByteArrayElements(character, arr, 0);
		return rc;
	}

	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoPlayerSet
 * Signature: (III[BI)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoPlayerSet
  (JNIEnv *env, jobject object, jint grid, jint swid, jint cmd,
   jbyteArray buf, jint buflen)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(buf, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->PlayerSet(grid, swid, cmd,  (unsigned char *)arr, buflen);

		env->ReleaseByteArrayElements(buf, arr, 0);
		return rc;
	}

	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoPlayerGetControlData
 * Signature: (I[B)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoPlayerGetControlData
  (JNIEnv *env, jobject object, jint offset, jbyteArray buf)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(buf, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->PlayerGetControlData(0, 1, offset, (unsigned char *)arr);

		env->ReleaseByteArrayElements(buf, arr, 0);
		return rc;
	}

	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeHAutoPlayerSetControlData
 * Signature: (I[BI)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeHAutoPlayerSetControlData
  (JNIEnv *env, jobject object, jint offset, jbyteArray buf, jint buflen)
{
	jbyte *arr;
	int rc = -1;

	if (hauto_)
	{
		arr = env->GetByteArrayElements(buf, NULL);
		if (arr == NULL)
		{
			LOGE("Error : Get array's elements");
			return -1;
		}

		rc = hauto_->PlayerSetControlData(0, 1, offset,  (unsigned char *)arr, buflen);

		env->ReleaseByteArrayElements(buf, arr, 0);
		return rc;
	}

	return -1;
}

/*
 * System
 */

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeSystemChangeCamera
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeSystemChangeCamera
  (JNIEnv *env, jobject object, jint ch)
{
	LOGD("[TRC %s:%d]", __func__, __LINE__);
	if (system_)
	{
		return system_->ChangeCamera(ch);
	}
	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeSystemHWWatchdogEnable
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeSystemHWWatchdogEnable
	(JNIEnv *env, jobject object, jboolean on)
{
	if (system_)
	{
		return system_->EnableHWWatchdog(on);
	}
	return -1;
}

/*
 * Class:     com_raonix_effect4dapp_HA210
 * Method:    nativeSystemHWWatchdogRefresh
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_raonix_effect4dapp_HA210_nativeSystemHWWatchdogRefresh
	(JNIEnv *env, jobject object)
{
	if (system_)
	{
		return system_->RefreshHWWatchdog();
	}
	return -1;
}
