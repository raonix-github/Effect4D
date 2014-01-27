package com.raonix.effect4dapp;

import android.app.Application;
import android.util.Log;

public class Effect4DApplication extends Application
{
	private static final String LOG_TAG="Application";

	private HA210 mHA210;
	private PlayerStatus mPlayerStatus;
	private LightStatus mLightStatus;
	private float mVideoSync;
	private float mEffectSync;

	public Effect4DApplication()
	{
		// must initialize HA210
		byte [] devicenum = {1,1,1,1};
//		mHA210 = new HA210();
//		mHA210.init(this);
//		mHA210.setDeviceNum(devicenum);
//		mHA210.changeCamera(1);
		
		mPlayerStatus=PlayerStatus.STOP;
		mLightStatus=LightStatus.OFF;
		mVideoSync=0.0F;
		mEffectSync=0.0F;
	}
	
	public enum PlayerStatus
	{
		PLAY, PAUSE, STOP
	}
	
	public enum LightStatus
	{
		ON, OFF
	}
	
	public void setPlayerPlay()
	{
		if(mHA210!=null)
			mHA210.setPlayerPlay();
	}

	public void setPlayerStop()
	{
		if(mHA210!=null)
			mHA210.setPlayerStop();
	}

	public void setPlayerPause()
	{
		if(mHA210!=null)
			mHA210.setPlayerPause();
	}
	
	public PlayerStatus getPlayerStatus()
	{
		return mPlayerStatus;
	}

	public void setLightOn()
	{
		if(mHA210!=null)
			mHA210.setLightOne(1, 1, 1, 0);
	}

	public void setLightOff()
	{
		if(mHA210!=null)
			mHA210.setLightOne(1, 1, 0, 0);
	}
	
	public LightStatus getLightStatus()
	{
		return mLightStatus;
	}
	
	public void setVideoSync(float val)
	{
		// TODO:
		mVideoSync=val;
		Log.d(LOG_TAG, "Video Sync:"+val);
	}
	
	public float getVideoSync()
	{
		return mVideoSync;
	}

	public void setEffectSync(float val)
	{
		// TODO:
		mEffectSync=val;
		Log.d(LOG_TAG, "Effect Sync:"+val);
	}
	
	public float getEffectSync()
	{
		return mEffectSync;
	}
	
}
