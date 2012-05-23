package conn;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class TerminalInterface {

	private Socket sock;
	private PrintWriter out;
	private BufferedReader in;

	public TerminalInterface() {
		sock = null;
	}

	public boolean connectTo(String ip, int port) {
		if (sock != null) {
			try {
				sock.close();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
		try {
			sock = new Socket("127.0.0.1", 6100);
			out = new PrintWriter(sock.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			sock.close();
			sock = null;
			return true;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	public void sendMessage(Message msg) {
		if (sock != null) {
			out.write(msg.toSendFormat());
		}
	}

	public Message getMessage() {
		return null;
	}

	public String getDisplayContent() {
		
		return null;
	}

	public void sendKey() {
		
	}
}
