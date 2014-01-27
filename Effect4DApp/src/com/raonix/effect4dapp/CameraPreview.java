package com.raonix.effect4dapp;

import java.io.IOException;

import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {
    SurfaceHolder mHolder;
    Camera mCamera;
    
    CameraPreview(Context context) {
        super(context);
        
        mHolder = getHolder();
        mHolder.addCallback(this);
//        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    public void surfaceCreated(SurfaceHolder holder) {
        mCamera = Camera.open(0);
        try {
           mCamera.setPreviewDisplay(holder);
           mCamera.startPreview();
        } catch (IOException exception) {
            mCamera.release();
            mCamera = null;
            // TODO: add more exception handling logic here
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
//        Camera.Parameters parameters = mCamera.getParameters();
//        parameters.setPreviewSize(w, h);
//        mCamera.setParameters(parameters);
//        mCamera.startPreview();
    }
}
