package com.raonix.effect4dapp;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;

public class MainActivity extends Activity
{
	private static final String LOG_TAG="MainActivity";
	
	private Effect4DApplication mApp;
	private Handler mHandler;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
	    requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(
				WindowManager.LayoutParams.FLAG_FULLSCREEN | 0x80000000,
				WindowManager.LayoutParams.FLAG_FULLSCREEN | 0x80000000);

		setContentView(R.layout.activity_run);
		
		mApp=(Effect4DApplication) getApplicationContext();
		
		mHandler=new Handler();

		initView();
	}
	

	private ImageView mImgSetting;
	private Button mBtnRun;
	private Button mBtnStop;
	private Button mBtnPause;
	private Button mBtnLight;

	private ViewGroup mContent;
	private CameraPreview mPreview;
	
	private void initView()
	{
		mImgSetting=(ImageView) findViewById(R.id.menu_btn_home);
		mImgSetting.setImageResource(R.drawable.btn_setting);

		((ImageView) findViewById(R.id.menu_img_title))
				.setImageResource(R.drawable.menutitle_home);

		mContent=(ViewGroup) findViewById(R.id.sub_screen_content);

		mBtnRun=(Button) findViewById(R.id.menu_01);
		mBtnStop=(Button) findViewById(R.id.menu_02);
		mBtnPause=(Button) findViewById(R.id.menu_03);
		mBtnLight=(Button) findViewById(R.id.menu_04);
		
		mBtnRun.setBackgroundResource(R.drawable.btn_run);
		mBtnStop.setBackgroundResource(R.drawable.btn_stop);
//		mBtnPause.setVisibility(View.INVISIBLE);
		mBtnPause.setBackgroundResource(R.drawable.btn_pause);
		mBtnLight.setBackgroundResource(R.drawable.btn_light);

		mBtnRun.setSelected(false);
		mBtnStop.setSelected(true);
		mBtnLight.setSelected(false);

		
		mImgSetting.setOnClickListener(new View.OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					Intent intent = new Intent( MainActivity.this, SettingActivity.class );
					startActivity(intent);
				}
			});
		

		mBtnRun.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				mApp.setPlayerPlay();
			}
		});

		mBtnStop.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				mApp.setPlayerStop();
			}
		});

		mBtnLight.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				if(v.isSelected())
					mApp.setLightOff();
				else
					mApp.setLightOn();
			}			
		});
	}

	@Override
	protected void onPause()
	{
		stopCameraPreview();
		super.onPause();
	}

	@Override
	protected void onResume()
	{
		super.onResume();
		startCameraPreview();
	}
	
	

	private Runnable mResurveStartPreview= new Runnable()
				{
					@Override
					public void run()
					{
						startCameraPreview();
					}
				};
	
	
	private void startCameraPreview()
	{
		if(mPreview!=null) return;
		if(mApp.isInitialized())
		{
			mPreview = new CameraPreview(this);
			mContent.addView(mPreview);
		}
		else
		{
			TRACE("HA210 not initialized.");
			mHandler.postDelayed(mResurveStartPreview, 1000);
		}
	}
	
	private void stopCameraPreview()
	{
		if(mPreview==null) return;

		// 시작하는 모든 메시지는 삭제.
		mHandler.removeCallbacks(mResurveStartPreview);

		mContent.removeView(mPreview);
		mPreview=null;
	}


	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	
	
	public static void TRACE(String msg)
	{
		if(msg==null) msg="";

		String claz=Thread.currentThread().getStackTrace()[3].getClassName();

//		Log.d(claz.substring(claz.lastIndexOf(".")+1),
//				String.format("[%04d:%s] %s",
//						Thread.currentThread().getStackTrace()[3].getLineNumber(),
//						Thread.currentThread().getStackTrace()[3].getMethodName(),
//						msg) );

		Log.d(claz.substring(claz.lastIndexOf(".")+1),
				String.format("[%04d] %s",
						Thread.currentThread().getStackTrace()[3].getLineNumber(),
						msg) );
	}

}
