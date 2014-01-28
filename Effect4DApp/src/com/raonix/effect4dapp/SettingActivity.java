package com.raonix.effect4dapp;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Context;
import android.util.Log;
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
	private Handler mHandler;

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
		
		mHandler=new Handler();
		
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
	private Button mBtnUpload;
	private Button mBtnDownload;
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
		mBtnUpload=(Button) findViewById(R.id.btn_upload);
		mBtnUpload.setOnClickListener(new OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					new FileUploadTask().execute();
				}
			});

		mBtnDownload=(Button) findViewById(R.id.btn_download);
		mBtnDownload.setOnClickListener(new OnClickListener()
			{
				@Override
				public void onClick(View v)
				{
					new FileDownloadTask().execute();
				}
			});
		

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
	
	
	private ProgressDialogFragment showProgressDialog(String title, String message)
	{
		// DialogFragment.show() will take care of adding the fragment
		// in a transaction.  We also want to remove any currently showing
		// dialog, so make our own transaction and take care of that here.
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		Fragment prev = getFragmentManager().findFragmentByTag("progress_dialog");
		if (prev != null)
			ft.remove(prev);

		ft.addToBackStack(null);
		// Create and show the dialog.
		ProgressDialogFragment newFragment=
				ProgressDialogFragment.newInstance(title, message, 100, 0);
		newFragment.show(ft, "progress_dialog");	
		return newFragment;
	}

	private void refresh()
	{
		String ele=getResources().getString(R.string.ele_sync);
		mSyncEffect.setText(Float.toString(mApp.getEffectSync())+ele);
		mSyncVideo.setText(Float.toString(mApp.getVideoSync())+ele);
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
	
	private class FileUploadTask extends AsyncTask<Void, Integer, String>
	{
		private ProgressDialogFragment progress;
		private FileOutputStream upload;

		public FileUploadTask()
		{
			// do nothing
		}


		@Override
		protected void onPreExecute()
		{
			super.onPreExecute();
			progress=showProgressDialog(null, "File upload.");

			File up=new File("/mnt/sdcard/effect4d/upload.dat");
			if(!up.exists())
			{
				try
				{
					if(!up.getParentFile().exists())
						up.getParentFile().mkdirs();
					up.createNewFile();
				}
				catch (IOException e1) { e1.printStackTrace(); }
			}

			try { upload=new FileOutputStream(up); }
			catch (FileNotFoundException e) { e.printStackTrace(); }
		}

		@Override
		protected void onPostExecute(final String result)
		{
			super.onPostExecute(result);
			progress.setStatusMessage(result);
			try { Thread.sleep(3000); }
			catch (InterruptedException e) {}	
			progress.dismiss();
		}


		@Override
		protected String doInBackground(Void... params)
		{
			if(upload==null) return new String("Can't create file.");

			int max=mApp.getPlayerUplaodSize();
			int cur=0;
			
			byte[] data=new byte[max];

			while(cur<max)
			{
				// TODO: 삭제할것
				try { Thread.sleep(10); }
				catch (InterruptedException e) { }
				
				int ret=mApp.playerUpload(cur, data);
				
				// TODO: 수정할것
				if(ret<1) cur+=10;
				else cur+=ret;

				this.publishProgress((int)((float)cur/(float)max*100F));
			}

			try
			{
				upload.write(data);
				upload.close();
			}
			catch (IOException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			return new String("File upload finished.");
		}

		@Override
		protected void onProgressUpdate(Integer... values)
		{
			super.onProgressUpdate(values);
			progress.setProgress(values[0].intValue());
		}
		
	}


	private class FileDownloadTask extends AsyncTask<Void, Integer, String>
	{
		private ProgressDialogFragment progress;
		private FileInputStream download;

		public FileDownloadTask()
		{
			// do nothing
		}


		@Override
		protected void onPreExecute()
		{
			super.onPreExecute();
			progress=showProgressDialog(null, "File download.");

			File down=new File("/mnt/sdcard/effect4d/download.dat");

			try { download=new FileInputStream(down); }
			catch (FileNotFoundException e) { e.printStackTrace(); }
		}

		@Override
		protected void onPostExecute(String result)
		{
			super.onPostExecute(result);
			progress.setStatusMessage(result);
			try { Thread.sleep(3000); }
			catch (InterruptedException e) {}
			progress.dismiss();
		}


		@Override
		protected String doInBackground(Void... params)
		{
			this.publishProgress(0);
			if(download==null) return new String("Can't open file.");
			
			int max=0;
			byte[] data;
			try
			{
				max=download.available();
				data=new byte[max];
				
				download.read(data);
				download.close();
			}
			catch (IOException e1)
			{
				e1.printStackTrace();
				return new String("File read error.("+e1.getMessage()+")");
			}
			
			int cur=0;
			while(cur<max)
			{
				// TODO: 삭제할것
				try { Thread.sleep(10); }
				catch (InterruptedException e) { }
				
				int ret=mApp.playerDownload(cur, data);

				// TODO: 수정할것
				if(ret<1)cur+=10;
				else cur+=ret;
				
				this.publishProgress((int)((float)cur/(float)max*100F));
			}

			return new String("File upload finished.");
		}

		@Override
		protected void onProgressUpdate(Integer... values)
		{
			super.onProgressUpdate(values);
			progress.setProgress(values[0].intValue());
		}
	}
}
