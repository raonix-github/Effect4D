package com.raonix.effect4dapp;

import java.io.UnsupportedEncodingException;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;

public class MainActivity extends Activity {
	
	HA210 mHA210;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	    requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN | 0x80000000,
				WindowManager.LayoutParams.FLAG_FULLSCREEN | 0x80000000);

//		setContentView(R.layout.activity_main);
		setContentView(R.layout.activity_run);
		
		initView();

		// must initialize HA210
		byte [] devicenum = {1,1,1,1};
		mHA210 = new HA210();
		mHA210.init(getApplicationContext());
		mHA210.setDeviceNum(devicenum);
		mHA210.changeCamera(1);
		
		
		// Light On/Off Test
		// TODO
		mHA210.setLightOne(1, 1, 1, 0);
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		mHA210.setLightOne(1, 1, 0, 0);
		
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
//		mImgSetting.setBackgroundResource(R.drawable.btn_setting);
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
		mBtnPause.setBackgroundResource(R.drawable.btn_pause);
		mBtnLight.setBackgroundResource(R.drawable.btn_light);
		
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
//				if(mHA210.setPlayerPlay() > 0) {
//				}
				mBtnRun.setSelected(true);
				mBtnStop.setSelected(false);
				mBtnPause.setSelected(false);
				mHA210.setPlayerPlay();
			}
		});

		mBtnStop.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				mBtnRun.setSelected(false);
				mBtnStop.setSelected(true);
				mBtnPause.setSelected(false);
				mHA210.setPlayerStop();
			}
		});

		mBtnPause.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				mBtnRun.setSelected(false);
				mBtnStop.setSelected(false);
				mBtnPause.setSelected(true);
				mHA210.setPlayerPause();
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

// layout���� �̹� ���� �����Ǿ� �ֽ�.
//		FrameLayout.LayoutParams params=
//				(FrameLayout.LayoutParams) mPreview.getLayoutParams();
//		int m=getResources().getDimensionPixelSize(R.dimen.MarginDefault);
//		params.setMargins(m, m, m, m);
//		mPreview.setLayoutParams(params);
	}
	
	private void stopCameraPreview()
	{
		if(mPreview==null) return;

		mContent.removeView(mPreview);
		mPreview=null;
	}


	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
}
