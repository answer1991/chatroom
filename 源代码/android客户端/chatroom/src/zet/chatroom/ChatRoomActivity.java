package zet.chatroom;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Toast;

public class ChatRoomActivity extends Activity
{
	private EditText chatlist;			//聊天记录
	private EditText userlist;			//用户列表
	private EditText message;			//信息输入框
	private Button   sendBtn;			//发送按钮
	private Button 	 clearBtn;			//清空聊天记录按钮
	private CheckBox IsPrivate;			//是否私聊选择框
	private EditText toUserText;		//私聊用户名输入框
	private Socket s;				
	private InputStream inStream;
	private OutputStream outStream;
	private RefleshHandler recvhandler = new RefleshHandler(); 
	private RecvThread recvThread = new RecvThread();
	private String ipaddr  ;      //服务器IP地址
	private String username ;     //用户名
	//private ExecutorService executorService;//线程池
	
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.roomlayout);
		
		chatlist = (EditText)findViewById(R.id.chatlist);
		userlist = (EditText)findViewById(R.id.userlist);
		message = (EditText)findViewById(R.id.messageText);
		sendBtn = (Button)findViewById(R.id.sendBtn);
		clearBtn = (Button)findViewById(R.id.clearBtn);
		IsPrivate = (CheckBox)findViewById(R.id.IsPrivate);
		toUserText = (EditText)findViewById(R.id.toUserText);
		
		sendBtn.setOnClickListener(new sendBtnListener());
		clearBtn.setOnClickListener(new clearBtnListener());
		IsPrivate.setOnCheckedChangeListener(new IsPrivateListener());
		
		//设置chatlist , userlist 和 toUserText只读;
		chatlist.setCursorVisible(false);
		chatlist.setFocusable(false);
		chatlist.setFocusableInTouchMode(false);
		userlist.setCursorVisible(false);
		userlist.setFocusable(false);
		userlist.setFocusableInTouchMode(false);
		toUserText.setCursorVisible(false);
		toUserText.setFocusable(false);
		toUserText.setFocusableInTouchMode(false);
		
		Intent intent = this.getIntent();
		ipaddr = intent.getStringExtra("ipaddr");
		username = intent.getStringExtra("username");
		
		//注册信息
		String loginInfo = "0\n" + username + "\n" + "0\n" + "0";
		//chatlist.append("ipaddr = " + ipaddr + " username = " + username);
		
		try 
		{
			s = new Socket(ipaddr,3204);
			//s  new Socket("10.0.2.2" , 3204);         
			outStream = s.getOutputStream();
			inStream = s.getInputStream();
			//发送注册信息
			outStream.write(loginInfo.getBytes("GBK"));
			//开启接收线程
			recvThread.start();
		} 
		catch (UnknownHostException e) 
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
			ChatRoomActivity.this.finish();
		} 
		catch (IOException e) 
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
		//recvhandler.post(recvRunable);
	}
	
	//接收线程
	public class RecvThread extends Thread
	{
		@Override
		public void run() {
			// TODO Auto-generated method stub
			while(true)
			{
				try 
				{
					if (inStream.available() > 0)
					{
						//接收byte
						byte[] 	recvByte = new byte[1024*2];
						int recvCount;
						if ((recvCount = inStream.read(recvByte)) < 0)
						{
							break;
						}
						//将受到的byte转成可读行的字符串数组
						//Message msg = recvhandler.obtainMessage(1, 1, 1, (Object)(new String(recvByte , 0 , recvCount ,"GBK" )));
						//recvhandler.sendMessage(msg);
						String[] cmdStr = recvByteToString(recvByte , recvCount);
						//System.out.println(data);
						//System.out.println(data.length());
						//发送消息给recvhandler 让handler来处理
						//把一个个以\0结尾的字符串协议用msg形式发给handler，让其来处理；
						//优化算法
						int i = 0 ;
						for(i = 0 ; i < 10 ; i ++)
						{
							if(cmdStr[i].compareTo("") != 0)
							{
								Message msg = recvhandler.obtainMessage(1, 1, 1, (Object)cmdStr[i]);
								recvhandler.sendMessage(msg);
								//chatlist.append(cmdStr[i]);
							}
							else 
								break;
							//chatlist.append(Integer.toString(i));
						}
						//如果instream有两个或者以上协议命令包，只会取到第一个
						/*if (recvCount(recvByte) > 0)
						{
							String recvStr = new String(recvByte , 0 , recvCount(recvByte) , "GBK");
							Message msg = recvhandler.obtainMessage(1, 1, 1, (Object)recvStr);
							recvhandler.sendMessage(msg);
						}*/
					}	
				} catch (Exception e) 
				{
					// TODO: handle exception
					//ChatRoomActivity.this.finish();
				}
			}
		}
	}
	
	//声明刷新聊天记录handler
	public class RefleshHandler extends Handler
	{
		@Override
		public void handleMessage(Message msg)
		{
			// TODO Auto-generated method stub
			super.handleMessage(msg);
			if(msg.what != 1)
				return ;
			String str = (String)msg.obj;
			String[] splitStr = str.split("\n" , 4);
			if (splitStr.length != 4)
			{
				//System.out.println(splitStr[0]);
				return ;
			}
			String type = splitStr[0];
			String fromuser = splitStr[1];
			String touser = splitStr[2];
			String data = splitStr[3];
			//当前时间
			SimpleDateFormat df = new SimpleDateFormat("HH:mm:ss");
			String timeStr = df.format(new Date());
			
			//分析判断信息type的类型
			//type == 1代表是群发
			if (type.compareTo("1") == 0 )
			{
				chatlist.append(timeStr + " ");
				chatlist.append(fromuser + " 对所有人说: ");
				chatlist.append(data + "\n");
			}
			//type == 2 表示私聊
			if (type.compareTo("2") == 0)
			{
				if (fromuser.compareTo(username) == 0)
				{
					chatlist.append(timeStr + " ");
					chatlist.append("您对 " + touser + " 说: ");
					chatlist.append(data + "\n");	
				}
				if (touser.compareTo(username) == 0)
				{
					chatlist.append(timeStr + " ");
					chatlist.append(fromuser + " 对您说: ");
					chatlist.append(data + "\n");				
				}
			}

			//type == 10 表示注册成功
			if (type.compareTo("10") == 0)
			{
				chatlist.append(timeStr + " ");
				chatlist.append("来自服务器的消息: ");
				chatlist.append(data + "\n");	
			}
			//type == 11 表示注册失败，该用户名已被别人注册
			if (type.compareTo("11") == 0)
			{
				chatlist.append(timeStr + " ");
				chatlist.append("来自服务器的消息: ");
				chatlist.append(data + "\n");
			}
			//type == 12 表示更新用户列表
			if (type.compareTo("12") == 0)
			{
				userlist.getText().clear();
				userlist.append(data);
				userlist.append("更新时间: \n" + timeStr);
			}
			//type == 13表示有用户进入聊天室
			if (type.compareTo("13") == 0)
			{
				chatlist.append(timeStr + " ");
				chatlist.append(data + "\n");
			}
			//type == 14表示发送的私聊信息出错
			if (type.compareTo("14") == 0)
			{
				chatlist.append(timeStr + " ");
				chatlist.append(data + "\n");
			}
			//System.out.println(type);
		}
	}
	
	//声明接收线程 ;这种方法使用handler开启的线程，优先级太高，会使整个Activity处于阻塞状态;
	//因此不用这种方法
	//改用handler处理消息机制，如果接收到数据，就发送一个消息给handler，然后再由handler对接收的字符串做处理;
	class RecvRunable implements Runnable
	{
		public void run() 
		{
			// TODO Auto-generated method stub	
			try 
			{
				if (inStream.available() > 0)
				{
				byte[] readBuff = new byte[1024*2];
				if (inStream.read(readBuff) < 0 )
					return;
				String data = new String(readBuff ,0 , recvCount(readBuff) , "GBK");
				System.out.println(data);
				System.out.println(data.length());
				chatlist.append(data);
				}	
				recvhandler.postDelayed(recvRunable, 2000);
			} 
			catch (IOException e) 
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	};
	private RecvRunable recvRunable = new RecvRunable();
	
	@Override
	protected void onStop() 
	{
		// TODO Auto-generated method stub
		super.onStop();
		try 
		{
			s.close();
		} 
		catch (IOException e) 
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Toast.makeText(ChatRoomActivity.this, "已和服务器断开连接!", Toast.LENGTH_LONG).show();
		recvThread.stop();
	}
	@Override
	protected void onDestroy() 
	{
		// TODO Auto-generated method stub
		super.onDestroy();
		try 
		{
			s.close();
		} catch (IOException e) 
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		recvThread.stop();
	}
	
	//发送按钮监听器复写
	public class sendBtnListener implements OnClickListener
	{
		@Override
		public void onClick(View v) 
		{
			// TODO Auto-generated method stub
			if (message.getText().toString().length() == 0)
			{
				Toast.makeText(ChatRoomActivity.this, "不能发送空信息!", Toast.LENGTH_LONG).show();
				return ;
			}
			//如果是私聊
			if(IsPrivate.isChecked())
			{
				if (toUserText.getText().length() == 0)
				{
					Toast.makeText(ChatRoomActivity.this, "请输入您要私聊的用户名!", Toast.LENGTH_LONG).show();
					return ;
				}
				try {
					if(toUserText.getText().toString().getBytes("GBK").length > 18)
					{
						Toast.makeText(ChatRoomActivity.this, "私聊的用户名太长!", Toast.LENGTH_LONG).show();
						return ;
					}
				} catch (UnsupportedEncodingException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				String sendStr = "2\n" + username + "\n" + toUserText.getText().toString() + "\n" + message.getText().toString();

				//System.out.println(sendStr);
				try 
				{
					byte[] sendByte = sendStr.getBytes("GBK");
					outStream.write(sendByte);
					
				} 
				catch (Exception e) 
				{
					// TODO: handle exception
					ChatRoomActivity.this.finish();
				}
			}
			else 
			{
				String sendStr = "1\n" + username + "\n0\n" + message.getText().toString() ;

				try 
				{
					byte[] sendByte = sendStr.getBytes("GBK");
					outStream.write(sendByte);
	
				} 
				catch (Exception e) 
				{
					// TODO: handle exception
					ChatRoomActivity.this.finish();
				}
			}
			message.getText().clear();
		}
	}
	
	//清空按钮监听器
	public class clearBtnListener implements OnClickListener
	{
		@Override
		public void onClick(View v) 
		{
			// TODO Auto-generated method stub
			chatlist.getText().clear();
		}
	}
	
	//私聊选择按钮监听器
	public class IsPrivateListener implements android.widget.CompoundButton.OnCheckedChangeListener
	{
		@Override
		public void onCheckedChanged
		(CompoundButton buttonView, boolean isChecked) 
		{
			// TODO Auto-generated method stub
			if (isChecked)
			{
				//toUserText 可写
				toUserText.setCursorVisible(true);
				toUserText.setFocusable(true);
				toUserText.setFocusableInTouchMode(true);
			}
			else
			{
				toUserText.setCursorVisible(false);
				toUserText.setFocusable(false);
				toUserText.setFocusableInTouchMode(false);
			}
		}
	}
	//计算从socket inputstream中读取多少个字符
	public static int recvCount(byte[] in)
	{
		int i = 0;
		for (i = 0 ; i < in.length ; i ++)
		{
			if (in[i] == 0)
				return i;
		}
		return in.length;
	}
	//优化算法，从instream里得到协议命令字符串数组;
	public static String[] recvByteToString(byte[] in , int recvCount)
	{
		String[] cmdStrArr = new String[10];
		int i = recvCount;
		int k;
		int strCount = 0;
		int offset = 0;
		for (k = 0 ; k < i ; k ++)
		{
			if (in[k] == 0)
			{
				try 
				{
					cmdStrArr[strCount] = new String(in ,offset , k - offset , "GBK");
				} 
				catch (UnsupportedEncodingException e) 
				{
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				strCount ++;
				offset = k + 1;
			}
		}
		//System.out.println(strCount);
		return cmdStrArr;
	}
	//用以计算Cmd的个数
	public static int cmdCount(String[] cmdStrArr)
	{
		String str = new String();
		int i;
		for (i = 0 ; i < cmdStrArr.length ; i ++)
		{
			if (str.compareTo(cmdStrArr[i]) == 0)
				return i;
		}
		return cmdStrArr.length;
	}
}
