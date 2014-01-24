package com.raonix.effect4dapp;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;

public class MainActivity extends Activity {
	
	HA210 mHA210;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

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

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
