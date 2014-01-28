package com.raonix.effect4dapp;

import android.app.DialogFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

public class ProgressDialogFragment extends DialogFragment
{
	/**
	 * 프로그레스 다이얼로그 프레그먼트를 생성한다.
	 * @param title 상단에 표시할 제목
	 * @param message 프로그레스바 위쪽에 표시할 메시지.
	 * @param max 베이스값 (이값에대한 백분율)
	 * @param mode 프로그레스바 왼쪽에 표시할 숫자형태. 0:표시하지 않음, 1:퍼센트표시, 2:분수표시
	 * @return 생성한 인스턴스
	 */
	static ProgressDialogFragment newInstance(
			String title, String message, int max, int mode)
	{
		ProgressDialogFragment f = new ProgressDialogFragment();

		// Supply num input as an argument.
		Bundle args = new Bundle();
		if(title!=null)
			args.putString("title", title);
		if(message!=null)
			args.putString("message", message);
		args.putInt("max", max);
		args.putInt("mode", mode);
		f.setArguments(args);

		return f;
	}
	
	private String mTitle;
	private String mMessage;
	private int mMax;
	private int mMode;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        int style=DialogFragment.STYLE_NORMAL, theme = 0;

        mTitle=getArguments().getString("title");
        if(mTitle==null)
        	style=DialogFragment.STYLE_NO_TITLE;
        
        setStyle(style, theme);
        
        mMessage=getArguments().getString("message");
        mMax=getArguments().getInt("max");
        mMode=getArguments().getInt("mode");
    }

    
    private ProgressBar mvwProgress;
    private TextView mvwProgressStatus;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
    	if(mTitle!=null)
    		getDialog().setTitle(mTitle);

        View v = inflater.inflate(R.layout.progress_dialog_fragment, container, false);

        View tv = v.findViewById(R.id.txt_message);
        if(mMessage!=null)
        	((TextView)tv).setText(mMessage);
        else
        	tv.setVisibility(View.INVISIBLE);
        
        mvwProgress=(ProgressBar) v.findViewById(R.id.progressBar);
        mvwProgress.setMax(mMax);
        mvwProgress.setProgress(0);
        
        mvwProgressStatus=(TextView) v.findViewById(R.id.txt_progress_status_message);
        mvwProgressStatus.setText("");

        setCancelable(false);

        return v;
    }
    
    public void setProgress(int progress)
    {
    	if(mvwProgress!=null)
    		mvwProgress.setProgress(progress);
    }

    public void setProgress(int progress, String status)
    {
    	setProgress(progress);
    	setStatusMessage(status);
    }
    
    public void setStatusMessage(String statusMessage)
    {
    	if(statusMessage==null) return;
    	
    	if(mvwProgressStatus!=null)
    		mvwProgressStatus.setText(statusMessage);
    }
}
