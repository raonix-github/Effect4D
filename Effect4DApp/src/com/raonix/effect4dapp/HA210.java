package com.raonix.effect4dapp;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;

import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.os.RecoverySystem;
import android.preference.PreferenceManager;
import android.util.Log;

import com.raonix.effect4dapp.JSONParser;

public class HA210 {
	// ------------------------------ FIELDS ------------------------------

	//Event constants
	/* Event Types */
	public static final int HAUTO_EVENT = 0;
	public static final int INTERPHONE_nativeHAutoDeviceSetNumEVENT = 1;
	public static final int SYSTEM_EVENT = 2;
	public static final int SECURITY_EVENT = 3;

	// Remote Management constants
	public final static String SERVER_IP = "http://iunplug.co.kr";

	// url to update product
	private static final String url_system_info
		= SERVER_IP + "/php/ha210/get_software_sys_info.php";
	
	// url to update software
	private static final String url_software_info
		= SERVER_IP + "/php/ha210/get_software_app_info.php";
	
	private static String url_alive_product
		= SERVER_IP + "/php/ha210/alive_product.php";
	
	private final static String mSystemFile = "/cache/ha210_system.zip";
	private final static String mApkFile = "/sdcard/ha210.apk";

	private static final String TAG_SUCCESS = "success";
	private static final String TAG_DESC = "description";
	
	private static final String TAG_UPDATE = "update";
	private static final String TAG_VERSION = "version";
	private static final String TAG_FILENAME = "filename";	
	private static final String TAG_FILEURL = "fileurl";
	private static final String TAG_BUILDCODE = "filecode";
	
	private static final String TAG_PID = "pid";
	private static final String TAG_NAME = "name";
	private static final String TAG_EMAIL = "email";
	private static final String TAG_HWVER = "hwver";
	private static final String TAG_SYSTEMVER = "sysver";
	private static final String TAG_APP = "appver";
	
	private static final String APK_NAME="Effect4DApp";
	
	public static final int STATE_IDLE = 0;
	public static final int STATE_DOWNLOAD = 0x1;
	
	private final static String LOG_TAG = "HA210";
	private static final Object mLock = new Object();
	private boolean initialized;
	
	private Context mContext;
	
	SharedPreferences mPrefs;
    
	// --------------------------- CONSTRUCTORS ---------------------------

	public HA210() {
		synchronized ( mLock ) {
			Log.i( LOG_TAG, "loading native library ha210" );
			System.loadLibrary( "ha210" );
		}
	}

	// -------------------------- OTHER METHODS --------------------------

	public void init( Context context ) {
		if ( !initialized ) {
			nativeInit( context );
			
			mContext = context;
			initialized = true;
			
			mPrefs = PreferenceManager.getDefaultSharedPreferences(mContext);

			mApkInfo.getInfo();

			mSystemCurVersion = Build.ID;
			mSystemCurBuildcode = getSystemBuildcode(mSystemCurVersion);
		}
	}
	
	private int getSystemBuildcode(String version) {
		int s = version.indexOf("-");
		if(s == -1) return 0;
		return Integer.parseInt(version.substring(s+1, s+4));
	}
	

	public void release() {
		if ( initialized ) {
			initialized = false;
			nativeRelease();
		}
	}
	
	/*
	 * HAuto
	 */
	public int setDeviceNum(byte [] num) {
		return nativeHAutoDeviceSetNum(num);
	}
	
	public int getLightState(int grid, byte [] state) {
		return nativeHAutoLightGetState(grid, state);
	}
	
	public int scanLight() {
		return nativeHAutoLightScan();
	}
	
	public  int getLightCharacter(int grid, byte [] character) {
		return nativeHAutoLightGetCharacter(grid, character);
	}

	public int setLightOne(int grid, int swid, int on , int dim) {
		return nativeHAutoLightSetOne(grid, swid, on, dim);
	}

	public int setLightAll(int grid, int swid, int on) {
		return nativeHAutoLightSetAll(grid, swid, on);
	}
	
	public int scanIO() {
		return nativeHAutoIOScan();
	}
	
	public int getIOState(int grid, byte [] state) {
		return nativeHAutoIOGetState(grid, state);
	}

	public int getIOCharacter(int grid, byte [] character) {
		return nativeHAutoIOGetCharacter(grid, character);
	}
	
	public int setIORelay(int grid, int swid, int val, int mask) {
		return nativeHAutoIOSetRelay(grid, swid, val, mask);
	}
	
	///////////////////////////////////////////////
	// called by JNI when device state has changed
	///////////////////////////////////////////////
	protected void handleLightStateChanged( int grid, int swid, int len, byte state[] ) {	
		// TODO
		Log.d(LOG_TAG, "handleLightStateChanged");
	}

	protected void handlePlayerStateChanged( int grid, int swid, int len, byte state[] ) {
		// TODO
		Log.d(LOG_TAG, "handlePlayerStateChanged");
	}

	protected void handleIOStateChanged( int grid, int swid, int len, byte state[] ) {
		// TODO
		Log.d(LOG_TAG, "handleIOStateChanged");
	}
	///////////////////////////////////////////////
	
	/*
	 * System
	 */
	protected void handleSystemEvent( int event, String data ) {
		// TODO
	}
	
	public int changeCamera(int ch) {
		return nativeSystemChangeCamera(ch);
	}

	public int setHWWatchdogEnable(boolean on) {
		return nativeSystemHWWatchdogEnable(on);
	}
	
	public int setHWWatchdogRefresh() {
		return nativeSystemHWWatchdogRefresh();
	}
	
	/*
	 * Server
	 */
	private String mSystemNewVersion;
	private int mSystemNewBuildcode;
	private String mSystemCurVersion;
	private int mSystemCurBuildcode;
	private String mSystemDesc;
	private URL mSystemURL;

	private ApkInfo mApkInfo = new ApkInfo(APK_NAME);
	private String mApkNewVersion;
	private String mApkDesc;
	private URL mApkURL;
	private int mApkNewBuildcode;
	
	private int mState = STATE_IDLE;
	private double mDownloadProgress;
	
	public String getSystemCurVersion() {
		return mSystemCurVersion;
	}

	public String getApkCurVersion() {
		return mApkInfo.getVersion();
	}

	public void checkSystem() {
		synchronized(mLock) {
			if(mState == STATE_IDLE) {
				mUpdateHandler.sendEmptyMessage(UPDATE_CHECK_SYSTEM);
			} else {
//				handleServerEvent(ServerEvent.EVENT_UPDATING, "");
			}
		}
	}
	
	public void checkApk() {
		synchronized(mLock) {
			if(mState == STATE_IDLE) {
				mUpdateHandler.sendEmptyMessage(UPDATE_CHECK_APK);
			} else {
//				handleServerEvent(ServerEvent.EVENT_UPDATING, "");
			}
		}
	}
	
	public void downloadSystem() {
		synchronized(mLock) {
			if(mState == STATE_IDLE) {
				mUpdateHandler.sendEmptyMessage(UPDATE_DOWNLOAD_SYSTEM);
			} else {
//				handleServerEvent(ServerEvent.EVENT_UPDATING, "");
			}
		}
	}

	public void downloadApk() {
		synchronized(mLock) {
			if(mState == STATE_IDLE) {
				mUpdateHandler.sendEmptyMessage(UPDATE_DOWNLOAD_APK);
			} else {
//				handleServerEvent(ServerEvent.EVENT_UPDATING, "");
			}
		}
	}
	
	
	
	public void updateSystem() {
		new InstallAndRebootThread(mContext, new File(mSystemFile)).start();
	}
	
	private class InstallAndRebootThread extends Thread {
		private final Context mContext;
		private final File mFile;

		InstallAndRebootThread(Context context, File file) {
			mContext = context;
			mFile = file;
		}

		public void run() {
			try {
				RecoverySystem.installPackage(mContext, mFile);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	} 
	
	public void updateApk() {
       Intent intent = mContext.getPackageManager().getLaunchIntentForPackage("com.raonix.hyeanupdater");
       intent.putExtra("HyeanApkFileName", mApkFile);
       try
       {
    	   mContext.startActivity(intent);
       }
       catch(ActivityNotFoundException e)
       {
    	   Log.e(LOG_TAG, "Update Activity Not Found.");
       }
	}
	
	public double getDownloadProgress() {
		if(mState == STATE_DOWNLOAD) {
			return mDownloadProgress;
		} else {
			return -1;
		}
	}
	
	private void checkSystemVersion() {
		new Thread(new Runnable() {
			public void run() {

				int success;
				JSONParser jsonParser = new JSONParser();
				try {
//					handleServerEvent(ServerEvent.EVENT_UPDATE_CHECK_RUN,"");
					
					// Building Parameters
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair(TAG_HWVER, "1.0.0"));
					
					// getting JSON string from URL
					JSONObject json = jsonParser.makeHttpRequest(
							url_system_info, "GET", params);
					
					if(json == null) {
//						handleServerEvent(ServerEvent.EVENT_CHECK_ERROR, "HTTP Request error" );
						return;
					}

					// check your log for json response
					Log.d("JSON Response: ", json.toString());

					// json success tag
					success = json.getInt(TAG_SUCCESS);
					if (success == 1) {
						// successfully received product details
						JSONArray infomationObj = json.getJSONArray(TAG_UPDATE); // JSON Array

						// get first product object from JSON Array
						JSONObject info = infomationObj.getJSONObject(0);

						mSystemNewVersion = info.getString(TAG_VERSION);
						mSystemNewBuildcode = getSystemBuildcode(mSystemNewVersion);
//						mSystemDesc = info.getString(TAG_DESC);
						mSystemURL = new URL(info.getString(TAG_FILEURL));
						
						if(mSystemNewBuildcode > mSystemCurBuildcode) {
//							handleServerEvent(ServerEvent.EVENT_SYSTEM_UPDATE_AVAILABLE, "mSystemNewVersion" );
						} else {
//							handleServerEvent(ServerEvent.EVENT_NO_UPDATE, "" );
						}
					} else {
//						handleServerEvent(ServerEvent.EVENT_CHECK_ERROR, "Request JSON error");
					}
				} catch (JSONException e) {
//					handleServerEvent(ServerEvent.EVENT_CHECK_ERROR, "Request JSON error");
					e.printStackTrace();
				} catch (MalformedURLException e) {
//					handleServerEvent(ServerEvent.EVENT_BAD_URL_ERROR, "URL error");
					e.printStackTrace();
				}
			}
		}).start();
	}
	
	private void checkApkVersion() {
		new Thread(new Runnable() {
			public void run() {

				int success;
				JSONParser jsonParser = new JSONParser();
				try {
//					handleServerEvent(ServerEvent.EVENT_UPDATE_CHECK_RUN,"");
					
					// Building Parameters
					List<NameValuePair> params = new ArrayList<NameValuePair>();
					params.add(new BasicNameValuePair(TAG_HWVER, "1.0.0"));
					
					// getting JSON string from URL
					JSONObject json = jsonParser.makeHttpRequest(
							url_software_info, "GET", params);
					
					if(json == null) {
//						handleServerEvent(ServerEvent.EVENT_CHECK_ERROR, "HTTP Request error" );
						return;
					}

					// check your log for json response
					Log.d("JSON Response: ", json.toString());

					// json success tag
					success = json.getInt(TAG_SUCCESS);
					if (success == 1) {
						// successfully received product details
						JSONArray infomationObj = json.getJSONArray(TAG_UPDATE); // JSON Array

						// get first product object from JSON Array
						JSONObject info = infomationObj.getJSONObject(0);

						mApkNewVersion = info.getString(TAG_VERSION);
						mApkNewBuildcode = info.getInt(TAG_BUILDCODE);
//						mApkDesc = info.getString(TAG_DESC);
						mApkURL = new URL(info.getString(TAG_FILEURL));
						
						if(mApkNewBuildcode > mApkInfo.getBuildcode()) {
//							handleServerEvent(ServerEvent.EVENT_APK_UPDATE_AVAILABLE, mApkNewVersion );
						} else {
//							handleServerEvent(ServerEvent.EVENT_NO_UPDATE, "" );
						}
					} else {
//						handleServerEvent(ServerEvent.EVENT_CHECK_ERROR, "Request JSON error");
					}
				} catch (JSONException e) {
//					handleServerEvent(ServerEvent.EVENT_CHECK_ERROR, "Request JSON error");
					e.printStackTrace();
				} catch (MalformedURLException e) {
//					handleServerEvent(ServerEvent.EVENT_BAD_URL_ERROR, "URL error");
					e.printStackTrace();
				}
			}
		}).start();
	}
	
	void downloadFile(final URL url, final String outfile, final Runnable callback) {
		new Thread(new Runnable() {
			public void run() {
				// Check for success tag
				int count;
				try {
					Log.d(LOG_TAG, "url: " + url.toString());
					Log.d(LOG_TAG, "outfile: " + outfile);
					
					URLConnection conection = url.openConnection();
					conection.connect();
					// getting file length
					int lenghtOfFile = conection.getContentLength();
					
					// input stream to read file - with 8k buffer
					InputStream input = new BufferedInputStream(url.openStream(), 8192);

					// Output stream to write file
					OutputStream output = new FileOutputStream(outfile);

					byte data[] = new byte[1024];

					long total = 0;

					while ((count = input.read(data)) != -1) {
						total += count;
						// publishing the progress....
						// After this onProgressUpdate will be called
						mDownloadProgress = (double)((total*100)/lenghtOfFile);

						// writing data to file
						output.write(data, 0, count);
					}

					// flushing output
					output.flush();
					
					// closing streams
					output.close();
					input.close();
					
				} catch (Exception e) {
					Log.e("Error: ", e.getMessage());
//					handleServerEvent(ServerEvent.EVENT_DOWNLOAD_ERROR, "Download error");
					mState = STATE_IDLE;
					return;
				}
				
				callback.run();
			}
		}).start();
	}
	

	////////////////////////////////////////////////////////////////////////////
	// APK Update 

	class PInfo {
		private String appname = "";
		private String pname = "";
		private String versionName = "";
		private int versionCode = 0;
		//private Drawable icon;
		/*private void prettyPrint() {
	            //Log.v(appname + "\t" + pname + "\t" + versionName + "\t" + versionCode);
	        }*/
	}	
	
	private class ApkInfo {
		int mBuildcode;
		String mVersion;
		String mName;
		
		ApkInfo(String apkname) {
			mName = apkname;
		}
		
		void getInfo() {
			ArrayList<PInfo> apps = getInstalledApps(false); /* false = no system packages */
			final int max = apps.size();
			for (int i=0; i<max; i++) {
				//apps.get(i).prettyPrint();        
				if(apps.get(i).appname.toString().equals(mName.toString())) {
					mVersion = apps.get(i).versionName.toString();
					mBuildcode = apps.get(i).versionCode;
					break;
				}
			}
		}
		
		String getVersion() {
			return mVersion;
		}

		int getBuildcode() {
			return mBuildcode;
		}
	}
	
	 // Get Information about Only Specific application which is Install on Device.
	public String getInstallPackageVersionInfo(String appName) 
    {
        String InstallVersion = "";     
        ArrayList<PInfo> apps = getInstalledApps(false); /* false = no system packages */
        final int max = apps.size();
        for (int i=0; i<max; i++) 
        {
            //apps.get(i).prettyPrint();        
            if(apps.get(i).appname.toString().equals(appName.toString()))
            {
                InstallVersion = "Install Version Code: "+ apps.get(i).versionCode+
                    " Version Name: "+ apps.get(i).versionName.toString();
                break;
            }
        }
        return InstallVersion.toString();
    }
    
    private ArrayList<PInfo> getInstalledApps(boolean getSysPackages) 
    {       
        ArrayList<PInfo> res = new ArrayList<PInfo>();        
        List<PackageInfo> packs = mContext.getPackageManager().getInstalledPackages(0);

        for(int i=0;i<packs.size();i++) 
        {
            PackageInfo p = packs.get(i);
            if ((!getSysPackages) && (p.versionName == null)) {
                continue ;
            }
            PInfo newInfo = new PInfo();
            newInfo.appname = p.applicationInfo.loadLabel(
            		mContext.getPackageManager()).toString();
            newInfo.pname = p.packageName;
            newInfo.versionName = p.versionName;
            newInfo.versionCode = p.versionCode;
            //newInfo.icon = p.applicationInfo.loadIcon(getPackageManager());
            res.add(newInfo);
        }
        return res; 
    }

    /*
	public void UnInstallApplication(String packageName)// Specific package Name Uninstall.
	{
		//Uri packageURI = Uri.parse("package:com.CheckInstallApp");
		Uri packageURI = Uri.parse(packageName.toString());
		Intent uninstallIntent = new Intent(Intent.ACTION_DELETE, packageURI);
		mContext.startActivity(uninstallIntent); 
	}
	
	public void InstallApplication(String pkg, String filename)
	{   
		Uri packageURI = Uri.parse(pkg.toString());
		Intent intent = new Intent(android.content.Intent.ACTION_VIEW, packageURI);

		intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		//intent.setFlags(Intent.ACTION_PACKAGE_REPLACED);
		//intent.setAction(Settings. ACTION_APPLICATION_SETTINGS);

		
		intent.setDataAndType
		(Uri.fromFile(new File(filename)), "application/vnd.android.package-archive");

		// Not open this Below Line Bcuz...
		////intent.setClass(this, Project02Activity.class); // This Line Call Activity Recursively its dangerous.

		mContext.startActivity(intent);  
	}
	*/
	
	/*
	 * Device Information
	 */
	
    private static final String FILENAME_PROC_VERSION = "/proc/version";
    private static final String FILENAME_MSV = "/sys/board_properties/soc/msv";
    
    /**
     * Reads a line from the specified file.
     * @param filename the file to read from
     * @return the first line, if any.
     * @throws IOException if the file couldn't be read
     */
    private String readLine(String filename) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(filename), 256);
        try {
            return reader.readLine();
        } finally {
            reader.close();
        }
    }

    private String getFormattedKernelVersion() {
        String procVersionStr;

        try {
            procVersionStr = readLine(FILENAME_PROC_VERSION);

            final String PROC_VERSION_REGEX =
                "\\w+\\s+" + /* ignore: Linux */
                "\\w+\\s+" + /* ignore: version */
                "([^\\s]+)\\s+" + /* group 1: 2.6.22-omap1 */
                "\\(([^\\s@]+(?:@[^\\s.]+)?)[^)]*\\)\\s+" + /* group 2: (xxxxxx@xxxxx.constant) */
                "\\((?:[^(]*\\([^)]*\\))?[^)]*\\)\\s+" + /* ignore: (gcc ..) */
                "([^\\s]+)\\s+" + /* group 3: #26 */
                "(?:PREEMPT\\s+)?" + /* ignore: PREEMPT (optional) */
                "(.+)"; /* group 4: date */

            Pattern p = Pattern.compile(PROC_VERSION_REGEX);
            Matcher m = p.matcher(procVersionStr);

            if (!m.matches()) {
                Log.e(LOG_TAG, "Regex did not match on /proc/version: " + procVersionStr);
                return "Unavailable";
            } else if (m.groupCount() < 4) {
                Log.e(LOG_TAG, "Regex match on /proc/version only returned " + m.groupCount()
                        + " groups");
                return "Unavailable";
            } else {
                return (new StringBuilder(m.group(1)).append("\n").append(
                		m.group(2)).append(" ").append(m.group(3)).append("\n")
                		.append(m.group(4))).toString();
            }
        } catch (IOException e) {
            Log.e(LOG_TAG,
                "IO Exception when getting kernel version for Device Info screen",
                e);

            return "Unavailable";
        }
    }

    /**
     * Returns " (ENGINEERING)" if the msv file has a zero value, else returns "".
     * @return a string to append to the model number description.
     */
    private String getMsvSuffix() {
        // Production devices should have a non-zero value. If we can't read it, assume it's a
        // production device so that we don't accidentally show that it's an ENGINEERING device.
        try {
            String msv = readLine(FILENAME_MSV);
            // Parse as a hex number. If it evaluates to a zero, then it's an engineering build.
            if (Long.parseLong(msv, 16) == 0) {
                return " (ENGINEERING)";
            }
        } catch (IOException ioe) {
            // Fail quietly, as the file may not exist on some devices.
        } catch (NumberFormatException nfe) {
            // Fail quietly, returning empty string should be sufficient
        }
        return "";
    }
    
	// -------------------------------------------------------------------------
	
	/////////////////////////////////////////////////////////////////////////////////////
	private static final int UPDATE_CHECK_SYSTEM = 0;
	private static final int UPDATE_CHECK_APK = 1;
	private static final int UPDATE_DOWNLOAD_SYSTEM = 2;
	private static final int UPDATE_DOWNLOAD_APK = 3;
	private static final int UPDATE_DOWNLOAD_SYSTEM_DONE = 4;
	private static final int UPDATE_DOWNLOAD_APK_DONE = 5;

	private UpdateHandler mUpdateHandler = new UpdateHandler();
	private boolean downloadapk;

	class UpdateHandler extends Handler {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
             
            switch (msg.what) {
            case UPDATE_CHECK_SYSTEM :
				checkSystemVersion();
                break;
            case UPDATE_CHECK_APK :
				checkApkVersion();
                break;
            case UPDATE_DOWNLOAD_SYSTEM :
				mState = STATE_DOWNLOAD;
				downloadFile(mSystemURL, mSystemFile, new Runnable () {
            		@Override
            		public void run() {
            			mUpdateHandler.sendEmptyMessage(UPDATE_DOWNLOAD_SYSTEM_DONE);
            		}
            	});
                break;
            case UPDATE_DOWNLOAD_APK :
				mState = STATE_DOWNLOAD;
            	downloadapk = false;
            	downloadFile(mApkURL, mApkFile, new Runnable () {
            		@Override
            		public void run() {
            			downloadapk = true;
            			mUpdateHandler.sendEmptyMessage(UPDATE_DOWNLOAD_APK_DONE);
            		}
            	});
                break;
            case UPDATE_DOWNLOAD_SYSTEM_DONE :
//				handleServerEvent(ServerEvent.EVENT_DOWNLOAD_SYSTEM_DONE, "");

				setHWWatchdogEnable(false);
				updateSystem();
            	break;
            case UPDATE_DOWNLOAD_APK_DONE :
            	if(downloadapk) {
					downloadapk = false;
//					handleServerEvent(ServerEvent.EVENT_DOWNLOAD_APK_DONE, "");

					setHWWatchdogEnable(false);
					updateApk();
            	}
            	break;
            default:
                break;
            }
        }
    };
    
    
    //
	// ------- Native Function -----------------------------------------------------------
    // 
	private native void nativeInit( Context context );
//	private native void nativeInit();
	private native void nativeRelease();

	// Home Automation - Common
	private native int nativeHAutoDeviceSetNum( byte [] num );
	
	// Home Automation - Light
	private native int nativeHAutoLightScan();
	private native int nativeHAutoLightGetState(int grid, byte [] state);
	private native int nativeHAutoLightGetCharacter(int grid, byte [] character);
	private native int nativeHAutoLightSetOne(int grid, int swid, int on, int dim);
	private native int nativeHAutoLightSetAll(int grid, int swid, int on);

	// Player
	private native int nativeHAutoPlayerScan();
	private native int nativeHAutoPlayerGetState(int grid, byte [] state);
	private native int nativeHAutoPlayerGetCharacter(int grid, byte [] character);
	private native int nativeHAutoPlayerSet(int grid, int swid, int cmd,
			byte [] buf, int buflen);

	// Motor Controller
	private native int nativeHAutoMotorControllerScan();
	private native int nativeHAutoMotorControllerGetState(int grid, byte [] state);
	private native int nativeHAutoMotorControllerGetCharacter(int grid, byte [] character);
	private native int nativeHAutoMotorControllerSet(int grid, int swid, int cmd,
			byte [] buf, int buflen);
	
	// Home Automation - IO
	private native int nativeHAutoIOScan();
	private native int nativeHAutoIOGetState(int grid, byte [] state);
	private native int nativeHAutoIOGetCharacter(int grid, byte [] character);
	private native int nativeHAutoIOSetRelay(int grid, int swid, int val, int mask);
	
	// System
	// H/W Watchdog
	private native int nativeSystemChangeCamera(int ch);
	private native int nativeSystemHWWatchdogEnable(boolean on);
	private native int nativeSystemHWWatchdogRefresh();
}