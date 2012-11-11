using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Text;
using namespace System::Net;
using namespace System::Net::Sockets;


public ref class ReceiveClass
	{
	public:
		System::Net::Sockets::Socket^ socket;
		System::Windows::Forms::RichTextBox^  chatlist;
		System::Windows::Forms::Button^  connectBtn;
		System::Windows::Forms::Button^  disconnectBtn;
		System::Windows::Forms::Button^  sendBtn;
		System::Windows::Forms::RichTextBox^  userlist;
		String^ username;
		System::Windows::Forms::TextBox^ usernameTextBox;
		System::Windows::Forms::TextBox^ ipaddrTextBox;
		ReceiveClass(Socket^ s ,RichTextBox^ c , Button^ Btn1 , Button^ Btn2 ,Button^ Btn3 , String^ u ,RichTextBox^  ul , TextBox^ usernameBox , TextBox^ ipaddr)
		{
			socket = s;
			chatlist = c;
			connectBtn = Btn1;
			disconnectBtn = Btn2;
			sendBtn = Btn3;
			username = u;
			userlist = ul;
			usernameTextBox = usernameBox;
			ipaddrTextBox = ipaddr;
		}
		void DoReceive()
		{
			int i;
			while(socket->Connected)
				{
					array<Byte> ^receiveByte = gcnew array<Byte>(2048);
					i = socket->Receive(receiveByte , receiveByte->Length ,static_cast<SocketFlags>(0));
					if (i > 0)
					{
						//chatlist->AppendText(Encoding::GetEncoding("GBK")->GetString(receiveByte));
						int offset =0;
						int ptr;
						for (ptr = 0 ; ptr < i ; ptr ++)
						{
							if (receiveByte[ptr] == 0)
							{
							String ^receiveStr = Encoding::GetEncoding("GBK")->GetString(receiveByte , offset , ptr - offset);
							//chatlist->AppendText(receiveStr);
							offset = ptr + 1 ;
							//分割收到的字符串
							array<wchar_t>^sep = gcnew array<wchar_t>{ '\n'};
							array<String^>^result = gcnew array<String^>(4);
							//chatlist->AppendText(receiveStr);
							result = receiveStr->Split(sep , 4 , StringSplitOptions::RemoveEmptyEntries );

						//收到的数据包符合协议
							if (result->Length >= 4)
							{
								String ^type = result[0];
								String ^fromusername = result[1];
								String ^tousername = result[2];
								String ^data = result[3];

							//chatlist->AppendText(type);

							//type = 1表示公聊，输出data
								if (type == "1")												
								{
									DateTime^ time = DateTime::Now;
									chatlist->AppendText(time->ToLongTimeString() + "  ");
									chatlist->AppendText(fromusername);
									chatlist->AppendText(" 对所有人说 : ");
									chatlist->AppendText(data);
									chatlist->AppendText("\n");
								}
							
							//type ==2 表示私聊,如果私聊对象为本机的用户，则输出，否则忽略
								if (type == "2")				
								{
								//chatlist->AppendText(username);
								//chatlist->AppendText(tousername);
									if (username == tousername)
									{
										//本机是接收方
										DateTime^ time = DateTime::Now;
										chatlist->AppendText(time->ToLongTimeString() + "  ");
										chatlist->AppendText(fromusername);
										chatlist->AppendText(" 对您说 : ");
										chatlist->AppendText(data);
										chatlist->AppendText("\n");
									}
									if (username == fromusername)
									{
									//本机是发送方
										DateTime^ time = DateTime::Now;
										chatlist->AppendText(time->ToLongTimeString() + "  ");
										chatlist->AppendText(fromusername);
										chatlist->AppendText(" 对 ");
										chatlist->AppendText(tousername);
										chatlist->AppendText(" 说 : ");
										chatlist->AppendText(data);
										chatlist->AppendText("\n");
									}
								}
							
							//type ==10 表示服务器接收注册的用户名
								else if (type == "10")									
								{
									DateTime^ time = DateTime::Now;
									chatlist->AppendText(time->ToLongTimeString() + "  ");
									chatlist->AppendText("来自服务器的信息 : ");
									chatlist->AppendText(data);
									chatlist->AppendText("\n");
								}
							
							//type = 11 表示请求注册名已被注册
								else if(type == "11")
								{
									DateTime^ time = DateTime::Now;
									chatlist->AppendText(time->ToLongTimeString() + "  ");
									chatlist->AppendText("来自服务器的信息 : ");
									chatlist->AppendText(data);
									chatlist->AppendText("\n");
								//socket->Disconnect(true);
								}
							
							//type == 12为更新用户列表
								else if(type == "12")
								{
									userlist->Text = data;
									DateTime^ time = DateTime::Now;
									userlist->AppendText("更新时间：" + time->ToLongTimeString() + "\n");
								}

							//type == 13为通知所有用户，某位用户加入了聊天室
								else if(type == "13")
								{
									DateTime^ time = DateTime::Now;
									chatlist->AppendText(time->ToLongTimeString() + "  ");
									chatlist->AppendText(data);
									chatlist->AppendText("\n");
								}

							//type == 14 私聊出错信息
								else if(type == "14")
								{
									DateTime^ time = DateTime::Now;
									chatlist->AppendText(time->ToLongTimeString() + "  ");
									chatlist->AppendText(data);
									chatlist->AppendText("\n");
								}
							}

							else  //接收到不符合协议的包
							{
							//chatlist->AppendText(receiveStr);
							}
							}
						}
					}

					else if(i <=0)
					{
						try
						{
							DateTime^ time = DateTime::Now;
							chatlist->AppendText(time->ToLongTimeString() + "  ");
							chatlist->AppendText("与服务器失去连接!\n");
							//socket->Disconnect(true);
							socket = gcnew Socket( AddressFamily::InterNetwork,SocketType::Stream,ProtocolType::Tcp );
							sendBtn->Enabled = false;
							disconnectBtn->Enabled = false;
							connectBtn->Enabled = true;
							usernameTextBox->ReadOnly = false;
							ipaddrTextBox ->ReadOnly = false;
							break;
						}
						catch (System::IO::IOException^ e)
						{
							MessageBox::Show(e->ToString());
						}
					}
				}
	   }

	};