package com.raonix.effect4dapp;

import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class SettingActivity extends Activity
{
	private static final float SYNC_STEP=0.5F;
	
	private LayoutInflater mInflater;
	private Effect4DApplication mApp;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
	    requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN | 0x80000000,
				WindowManager.LayoutParams.FLAG_FULLSCREEN | 0x80000000);
		setContentView(R.layout.activity_setting);

		mInflater=(LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		mApp=(Effect4DApplication) getApplicationContext();
		
		initView();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.setting, menu);
		return true;
	}
	
	
	private ImageView mImgHome;
	private Button mBtnGeneral;
	private Button mBtnList;
	private TextView mSyncVideo;
	private TextView mSyncEffect;
	
	private void initView()
	{
		mImgHome=(ImageView) findViewById(R.id.menu_btn_home);
//		mImgHome.setBackgroundResource(R.drawable.btn_home);
		mImgHome.setImageResource(R.drawable.btn_home);
		
		((ImageView) findViewById(R.id.menu_img_title))
				.setImageResource(R.drawable.menutitle_setting);

		mBtnGeneral=(Button) findViewById(R.id.menu_01);
		mBtnList=(Button) findViewById(R.id.menu_02);
		((Button)findViewById(R.id.menu_03)).setVisibility(View.INVISIBLE);
		((Button)findViewById(R.id.menu_04)).setVisibility(View.INVISIBLE);
		
		mBtnGeneral.setBackgroundResource(R.drawable.btn_basic);
		mBtnGeneral.setSelected(true);
		mBtnList.setBackgroundResource(R.drawable.btn_list);
		

		mImgHome.setOnClickListener(new View.OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					finish();
				}
			});

		LinearLayout content=(LinearLayout) findViewById(R.id.sub_screen_content);

		mInflater.inflate(R.layout.set_itm_dataupload, content);
		mInflater.inflate(R.layout.set_itm_datadownload, content);

		View v=mInflater.inflate(R.layout.set_itm_videosync, content);
		mSyncVideo=(TextView)v.findViewById(R.id.video_sync_delay);
		Button btn=(Button)v.findViewById(R.id.btn_video_minus);
		btn.setOnClickListener(new OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					mApp.setVideoSync(mApp.getVideoSync()-SYNC_STEP);
					refresh();
				}
			});

		btn=(Button)v.findViewById(R.id.btn_video_plus);
		btn.setOnClickListener(new OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					mApp.setVideoSync(mApp.getVideoSync()+SYNC_STEP);
					refresh();
				}
			});
		
		
		v=mInflater.inflate(R.layout.set_itm_effectsync, content);
		mSyncEffect=(TextView)v.findViewById(R.id.effect_sync_delay);
		btn=(Button)v.findViewById(R.id.btn_effect_minus);
		btn.setOnClickListener(new OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					mApp.setEffectSync(mApp.getEffectSync()-SYNC_STEP);
					refresh();
				}
			});

		btn=(Button)v.findViewById(R.id.btn_effect_plus);
		btn.setOnClickListener(new OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					mApp.setEffectSync(mApp.getEffectSync()+SYNC_STEP);
					refresh();
				}
			});
		
		refresh();
	}
	
	private void refresh()
	{
		String ele=getResources().getString(R.string.ele_sync);
		mSyncEffect.setText(Float.toString(mApp.getEffectSync())+ele);
		mSyncVideo.setText(Float.toString(mApp.getVideoSync())+ele);
	}
}
