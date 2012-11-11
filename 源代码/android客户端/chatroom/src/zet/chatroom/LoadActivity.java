package zet.chatroom;

import java.io.UnsupportedEncodingException;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class LoadActivity extends Activity {
    /** Called when the activity is first created. */
	private EditText IpaddrEdit ;
	private EditText UsernameEdit ;
	private Button LoginBtn ;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        IpaddrEdit  = (EditText)findViewById(R.id.IpaddrEdit);
    	UsernameEdit = (EditText)findViewById(R.id.UsernameEdit);
    	LoginBtn = (Button)findViewById(R.id.LoginBtn);
        LoginBtn.setOnClickListener(new LoginBtnListener());
    }
    
    public class LoginBtnListener implements OnClickListener
    {
    	@Override
    	public void onClick(View v) 
    	{
    		// TODO Auto-generated method stub
    		Intent intent = new Intent();
    		intent.putExtra("ipaddr", IpaddrEdit.getText().toString());
    		intent.putExtra("username", UsernameEdit.getText().toString());
    		intent.setClass(LoadActivity.this,ChatRoomActivity.class);
    		try {
				if (UsernameEdit.getText().toString().getBytes("GBK").length > 18)
				{
					Toast.makeText(LoadActivity.this, "您的用户名必须小于20字节!", Toast.LENGTH_LONG).show();
					return;
				}
			} catch (UnsupportedEncodingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    		LoadActivity.this.startActivity(intent);
    	}
    }
    
}