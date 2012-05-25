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

	public TerminalInterface(String ip, int port) {
		sock = null;
		connectTo(ip, port);
		System.out.println("Connect ok");
	}

	public boolean connectTo(String ip, int port) {
		if (sock != null) {
			try {
				sock.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}
		try {
			sock = new Socket(ip, port);
			out = new PrintWriter(sock.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			return true;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	public void sendMessage(Message msg) {
			String msgstring = msg.toSendFormat();
			out.println(msgstring);
			System.out.println("Message:\n"+msgstring+" sent");
	}

	/*public Message getMessage() {
		return null;
	}*/

	public String getDisplayContent() {
		Message getdisp = new Message("Give screen data to me!");
		sendMessage(getdisp);
		return "ok";
	}

	/*public void sendKey() {
		
	}*/
}
