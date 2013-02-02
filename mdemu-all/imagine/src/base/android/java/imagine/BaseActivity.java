/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

package com.imagine;

import android.app.*;
import android.content.*;
import android.content.res.Configuration;
import android.content.pm.ActivityInfo;
import android.os.*;
import android.view.*;
import android.graphics.*;
import java.io.*;
import android.util.*;
import android.hardware.*;
import java.util.List;
import android.media.*;

public final class BaseActivity extends Activity
{
	private static GLView glView;
	private static String logTag = "BaseActivity";

	static
	{
		//Log.i(logTag, "class init");
		
		/*try
		{
			SDK5Wrap.checkAvailable();
			sdk5Present = true;
			//Log.i(logTag, "SDK 5 present");
		} catch (Throwable t)
		{
			sdk5Present = false;
			//Log.i(logTag, "SDK 5 not present");
		}*/
		
		/*boolean hasNeon = false, hasVFP = false, isARMv6 = 0;
		try
		{
			BufferedReader buf = new BufferedReader(new FileReader("/proc/cpuinfo"), 2048);
			String line;
			while ((line = buf.readLine()) != null)
			{
				if(line.contains("Processor"))
				{
					//Log.i(logTag, "found processor line:" + line);
					if(line.toLowerCase().contains("armv6"))
					{
						Log.i(logTag, "is ARMv6");
						isARMv6 = true;
					}
				}
				else if(line.contains("Features"))
				{
					//Log.i(logTag, "found features line:" + line);
					if(line.contains("neon"))
					{
						Log.i(logTag, "has neon");
						hasNeon = true;
					}
					if(line.contains("vfp"))
					{
						Log.i(logTag, "has vfp");
						hasVFP = true;
					}
					break;
				}
			}
			buf.close();
		}
		catch (Exception e)
		{
			//Log.i(logTag, "error opening cpuinfo");
		}*/
		
		/*if(hasNeon)
		{
			System.loadLibrary("imagine-neon");
		}
		else if(isARMv6 && hasVFP)
		{
			System.loadLibrary("imagine-vfp");
		}
		else*/
		{
			System.loadLibrary("imagine");
		}
		//Log.i(logTag, "class init done");
	}

	private native void envConfig(String filePath, String ExtStoragePath, int xMM, int yMM, int orientation, int refreshRate, 
			int apiLevel, int hardKeyboardState, int navigationState, String devName, String apkPath, Vibrator sysVibrator);
	
	private static Display dpy;
	
	@Override protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		//Log.i(logTag, "onCreate");
		
		//getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		//requestWindowFeature(Window.FEATURE_NO_TITLE);
		//requestWindowFeature(Window.FEATURE_PROGRESS);
		/*Log.i(logTag, android.os.Build.BRAND + " " + android.os.Build.BOARD + " " +
		android.os.Build.DEVICE + " " + android.os.Build.DISPLAY + " " +
		 " " + android.os.Build.MANUFACTURER + " " +
		android.os.Build.MODEL + " " + android.os.Build.PRODUCT );*/

		dpy = getWindowManager().getDefaultDisplay();
		DisplayMetrics metrics = new DisplayMetrics();
		dpy.getMetrics(metrics);
		int xMM = (int)(((float)metrics.widthPixels / metrics.xdpi) * 25.4);
		int yMM = (int)(((float)metrics.heightPixels / metrics.ydpi) * 25.4);
		Context context = getApplicationContext();
		Vibrator vibrator = (Vibrator)context.getSystemService(Context.VIBRATOR_SERVICE);
		//Log.i(logTag, "refresh rate " + dpy.getRefreshRate());
		//Log.i(logTag, "rotation " + orientation);
		/*Log.i(logTag, "display pixels " + metrics.widthPixels + "x" + metrics.heightPixels
				+ " DPI " + metrics.xdpi + "x" + metrics.ydpi + " density " + metrics.density
				+ " density DPI " + metrics.densityDpi + " scaled density " + metrics.scaledDensity);*/
		Configuration config = getResources().getConfiguration();
		envConfig(context.getFilesDir().getAbsolutePath(), Environment.getExternalStorageDirectory().getAbsolutePath(),
				xMM, yMM, dpy.getOrientation(), (int)dpy.getRefreshRate(),
				android.os.Build.VERSION.SDK_INT, config.hardKeyboardHidden,
				SDKCompat.getNavigationHidden(config),
				android.os.Build.DEVICE, getApplicationInfo().sourceDir, vibrator);
		glView = new GLView(this);
		setContentView(glView);
	}
	
	private static native void appPaused();
	private static native void appResumed(boolean hasFocus);

	private static boolean isPaused = false;
	@Override protected void onPause()
	{
		//Log.i(logTag, "onPause");
		super.onPause();
		/*if(orientation.mEnabled)
		{
			//Log.i("BaseActivity", "disabling sensors in pause");
			orientationSensorPaused = true;
			orientation.disable(10);
		}
		else
			orientationSensorPaused = false;*/
		isPaused = true;
		if(glView.initNative)
		{
			if(glView.initSurface)
				glView.stopUpdate();
			appPaused();
		}
	}

	@Override protected void onResume()
	{
		//Log.i(logTag, "onResume");
		removeNotification();
		super.onResume();
		/*if(orientationSensorPaused)
		{
			orientation.enable();
		}*/
		if(glView.initNative)
		{
			if(glView.initSurface)
				glView.postUpdate();
			appResumed(hasWindowFocus());
		}
		isPaused = false;
	}
	
	private static native boolean appFocus(boolean hasFocus);
	@Override public void onWindowFocusChanged(boolean hasFocus)
	{
		super.onWindowFocusChanged(hasFocus);
		if(appFocus(hasFocus))
		{
			glView.postUpdate();
		}
	}
	
	/*@Override protected void  onStop()
	{
		super.onStop();
	}*/

	@Override protected void  onDestroy()
	{
		//Log.i(logTag, "onDestroy");
		super.onDestroy();
		removeNotification();
	}
	
	/*@Override public void onLowMemory()
	{
		Log.i(logTag, "onLowMemory");
		if(isPaused)
			removeNotification();
	}*/

	private static native void configChange(int keyboardState, int navigationState, int orientation);
	@Override public void onConfigurationChanged(Configuration newConfig)
	{
		//Log.i(logTag, "onConfigurationChanged");
		super.onConfigurationChanged(newConfig);
		//Log.i("BaseActivity", "config change, orientation: " + newConfig.orientation + " hard keyboard: " + newConfig.hardKeyboardHidden);
		configChange(newConfig.hardKeyboardHidden,
				SDKCompat.getNavigationHidden(newConfig),
				dpy.getOrientation());
		//setOrientationOS(dpy.getOrientation());
		//Log.i(logTag, "config change, navigation: " + newConfig.navigation + " navigationHidden: " + newConfig.navigationHidden);
	}

	/*public static void setStatusBar(byte on)
	{
		Window win = act.getWindow();
		if(on != 0)
			win.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		else
			win.clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
	}*/
	
	// events from threads
	private static native boolean handleAndroidMsg(int arg1, int arg2, int arg3);
	private static Handler msgHandler = new Handler()
	{
		@Override public void handleMessage(Message msg)
		{
			//Log.i(logTag, "got Msg " + msg.arg1);
			if(handleAndroidMsg(msg.what, msg.arg1, msg.arg2))
			{
				glView.postUpdate();
			}
		}
	};
	
	// timer callback
	private static native boolean timerCallback(boolean isPaused);
	private static Runnable timerCallbackRunnable = new Runnable()
	{
		public void run()
		{
			if(timerCallback(isPaused))
			{
				glView.postUpdate();
			}
		}
	};
	
	public void addNotification(String onShow, String title, String message)
	{
		NotificationHelper.addNotification(getApplicationContext(), onShow, title, message);
	}
	
	public void removeNotification()
	{
		NotificationHelper.removeNotification();
	}
	
	private static native boolean keyEvent(int key, int down);
	private static boolean allowKeyRepeats = true;
	private static boolean handleVolumeKeys = false;
	@Override public boolean dispatchKeyEvent(KeyEvent event)
	{
		int keyCode = event.getKeyCode();
		//Log.i(logTag, "got key " + keyCode + " " + event.getAction() + " repeat " + event.getRepeatCount());
		if(!handleVolumeKeys &&
			(keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN))
			return false;
		if(allowKeyRepeats || event.getRepeatCount() == 0)
		{
			if(keyEvent(keyCode, event.getAction() == KeyEvent.ACTION_UP ? 0 : 1))
			{
				glView.postUpdate();
			}
		}
		return true;
	}
	
	// Old Orientation support code
	// TODO: remove
	//private static boolean orientationSensorPaused = false;
	/*private static native void setOrientation(int o);
	private static OrientationHandler orientation;
	
	private static int surfaceRotationToNativeBit(int rotation)
	{
		switch(rotation)
		{
			case Surface.ROTATION_0: return 1;
			case Surface.ROTATION_90: return 2;
			case Surface.ROTATION_270: return 8;
			default : return 0;
		}
	}
	
	private static int nativeBitToSurfaceRotation(int bit)
	{
		switch(bit)
		{
			case 1 : return Surface.ROTATION_0;
			case 2 : return Surface.ROTATION_90;
			case 8 : return Surface.ROTATION_270;
			default : return 0;
		}
	}
	
	public static void onOrientationChanged(int rotation)
	{
		int bit = surfaceRotationToNativeBit(rotation);
		if(bit != 0)
		{
			setOrientation(bit);
		}
	}
	
	public static void setAutoOrientation(boolean on, int staticO)
	{
		if(on)
			orientation.enable();
		else
		{
			//Log.i("BaseActivity", "sensor disabled with o " + staticO);
			orientation.disable(nativeBitToSurfaceRotation(staticO));
		}
	}
	
	public static void setupOrientation()
	{
		orientation = new OrientationHandler((Context)act);
	}*/
}
