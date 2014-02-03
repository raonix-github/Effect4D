package com.raonix.effect4dapp;

import java.io.IOException;

import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

class CameraPreview extends SurfaceView implements SurfaceHolder.Callback
{
    SurfaceHolder mHolder;
    Camera mCamera;
    
    public CameraPreview(Context context, Camera camera)
    {
        super(context);
        mCamera=camera;
        mHolder=getHolder();
        mHolder.addCallback(this);
        //mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    public void surfaceCreated(SurfaceHolder holder)
    {
    	if(mCamera==null)
    	{
    		Toast.makeText(getContext(),
    				"Can not open camera.", Toast.LENGTH_SHORT).show();
    		return;
    	}
    	
        try
        {
           mCamera.setPreviewDisplay(holder);
           mCamera.startPreview();
        }
        catch (IOException exception)
        {
            mCamera.release();
            mCamera = null;
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder)
    {
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h)
    {
// FIXME: 여기서 작업을 할 경우 화면 크기지정이 정확하게 동작하지 않아 화면이 흐른다.
//        Camera.Parameters parameters = mCamera.getParameters();
//        parameters.setPreviewSize(w, h);
//        mCamera.setParameters(parameters);
//        mCamera.startPreview();
    }
}
