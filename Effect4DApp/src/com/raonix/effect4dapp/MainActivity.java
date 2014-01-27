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

		mBtnRun=(Button) findViewById(R.id.menu_01);
		mBtnStop=(Button) findViewById(R.id.menu_02);
		((Button)findViewById(R.id.menu_03)).setVisibility(View.INVISIBLE);
		mBtnLight=(Button) findViewById(R.id.menu_04);
		
		mBtnRun.setBackgroundResource(R.drawable.btn_run);
		mBtnStop.setBackgroundResource(R.drawable.btn_stop);
		mBtnLight.setBackgroundResource(R.drawable.btn_light);
		
		mContent=(ViewGroup) findViewById(R.id.sub_screen_content);
		
		mImgSetting.setOnClickListener(new View.OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					Intent intent = new Intent( MainActivity.this, SettingActivity.class );
					startActivity(intent);
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

// layout에서 이미 마진 지정되어 있슴.
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
