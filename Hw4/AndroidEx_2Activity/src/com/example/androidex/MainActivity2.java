package com.example.androidex;

import com.example.androidex.R;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;



public class MainActivity2 extends Activity {
	LinearLayout linear;
	EditText data;
	Button btn;
	Intent intent;
	OnClickListener ltn;
	OnClickListener ltn2;
	public int row,col;
	public int width,height;
	public int blank;
	public int[] key;
	public int fd;
	native public int open_fpga();
	native public int read_fpga(int fd);
	native public void write_fpga();
	native public int close_fpga(int fd);
	
	BackThread mThread;
	
	Handler mHandler=new Handler(){
		public void handleMessage(Message msg){
			if(msg.what==1){
				close_fpga(fd);
				finish();
			}
		}
	};
	class BackThread extends Thread{
		Handler sHandler;
		BackThread(Handler handler){
			sHandler=handler;
		}
		public void run(){
			while(true){
				Message msg=Message.obtain();
				msg.what=read_fpga(fd);
				sHandler.sendMessage(msg);
				try{Thread.sleep(1000);}catch(InterruptedException e){;}
			}
		}
	}
	
	public void swap(int ind1,int ind2)
	{
		int tmp;
		tmp = key[ind1];
		key[ind1] = key[ind2];
		key[ind2] = tmp;
	}
	public void make_button()
	{
		
		for(int i =0;i<row ;i++)
		{
			LinearLayout row_layout = new LinearLayout(this);
			row_layout.setLayoutParams(new LayoutParams(width,height/row));
			row_layout.setId(i);
			for(int j=0;j<col;j++)
			{
				Button btnTag = new Button(this);
				if(j+i*col == blank)
				{
					btnTag.setBackgroundColor(Color.BLACK);
					btnTag.setLayoutParams(new LayoutParams(width/col,height/row));
				}
				else
				{
					btnTag.setLayoutParams(new LayoutParams(width/col,height/row));
					btnTag.setText(String.valueOf(key[j+i*col]));
					if((j+i*col == blank-1 && (blank%col)!=0 ) ||
						(j+i*col == blank+1 && (blank%col)!=col-1) ||
						(j+i*col == blank-col && (blank/col)!=0 )||
						(j+i*col == blank+col &&  (blank/col)!=row-1))
					{
						
						ltn2 = new OnClickListener(){
							public void onClick(View V){
								Button b = (Button)V;
								int i;
								for(i=0;i<row*col;i++)
								{
									if(key[i] == Integer.parseInt((String)b.getText()))
										break;
								}
								swap(blank,i);
								blank=i;
								for(i=0;i<row;i++)
									linear.removeViewAt(2);
								write_fpga();
								make_button();
								for(i=0;i<row*col;i++)
								{
									if(key[i]!=i+1)
										break;
								}
								if(i==row*col)
								{
									key = null;
									close_fpga(fd);
									finish();
								}
							}
						};
						btnTag.setOnClickListener(ltn2);
					}
				
				}
				
				row_layout.addView(btnTag);
			}
			linear.addView(row_layout);
		}
	}
	public void getinput()
	{
	
		data=(EditText)findViewById(R.id.editText1);
		Button btn=(Button)findViewById(R.id.button1);
		height -= 150;
		ltn=new OnClickListener(){
			public void onClick(View v){
				char tmp1=0,tmp2=0;
				String temp=data.getText().toString();
				if(temp.length() == 3)
				{
					tmp1 = temp.charAt(0);
					tmp2 = temp.charAt(2);
					if(tmp1 > '1' && tmp1 < '5')
						row = tmp1-'0';
					if(tmp2 > '1' && tmp2 < '5')
						col = tmp2-'0';
				}
				//Log.v("tag",String.valueOf(row)+" "+String.valueOf(col));
				
				key = new int [row*col];
				for(int i=0;i<row*col;i++)
					key[i] = i+1;
				blank=row*col-1;
				//Log.v("tag",String.valueOf(blank));
				for(int i=0;i<row*col*10;i++)
				{
					int dir = (int)(Math.random()*4);
					if(dir == 0 && (blank%col)!=0)	//left
					{
						
						swap(blank,blank-1);
						blank-=1;
					}
					else if(dir ==1 && (blank/col)!=0)	//up
					{
						
						swap(blank,blank-col);
						blank-=col;
					}
					else if(dir==2 && (blank%col)!=col-1)	//right
					{
						
						swap(blank,blank+1);
						blank+=1;
					}
					else if( dir ==3 && (blank/col)!=row-1)		//down
					{
						
						swap(blank,blank+col);
						blank+=col;
					}
					else
						i-=1;
					
				}
				make_button();
			}
		};
				
		btn.setOnClickListener(ltn);
		
	}
	
		@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
		linear = (LinearLayout)findViewById(R.id.container);
		height = 540;
		width = 1024;
		row = col = 0;
		System.loadLibrary("main-activity2");
		fd = open_fpga();
		getinput();
		mThread=new BackThread(mHandler);
		mThread.setDaemon(true);
		mThread.start();
		
	}

}
