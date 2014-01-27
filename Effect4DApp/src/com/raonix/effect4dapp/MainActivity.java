package com.raonix.effect4dapp;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;

public class MainActivity extends Activity
{
	private Effect4DApplication mApp;
	
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
	
	private void startCameraPreview()
	{
		if(mPreview!=null) return;
		
		mPreview = new CameraPreview(this);
		mContent.addView(mPreview);
	}
	
	private void stopCameraPreview()
	{
		if(mPreview==null) return;

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
}
