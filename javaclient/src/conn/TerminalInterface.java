package conn;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

import data.javson.JSONObject;

public class TerminalInterface {

	private Socket sock;
	private PrintWriter sockout;
	private BufferedReader sockin;

	private String ip;
	private int port;

	public TerminalInterface(String ip, int port) {
		sock = null;
		this.ip = ip;
		this.port = port;

		boolean retval = connectTo(ip, port);
		System.out.println("Connect returned "+retval);
	}

	public boolean connectTo(String ip, int port) {
		if (sock != null) {
			try {
				sock.close();
			} catch (IOException e1) {
				//e1.printStackTrace();
			}
		}
		try {
			sock = new Socket(ip, port);
			sockout = new PrintWriter(sock.getOutputStream(), true);
			sockin = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			return true;
		} catch (Exception e) {
			//e.printStackTrace();
			sock = null;
			sockout = null;
			sockin = null;
		}
		return false;
	}

	public boolean reconnect() {
		return connectTo(ip, port);
	}

	public void sendMessage(Message msg) {
		boolean ok = true;
		if (sock == null || sockin == null) {
			ok = reconnect();
		}

		String msgstring = msg.toSendFormat();
		if (ok) {
			sockout.println(msgstring);
			System.out.println("Message:\n"+msgstring+" sent");
		} else {
			System.out.println("No connection, not sent");
		}
	}

	public String readAll() {
		boolean ok = (sock != null) && (sockin != null);
		if (ok) {
			String retstr = "";
			String str = "";
			do {
				try {
					str = sockin.readLine();
				} catch (Exception ex) {
					//ex.printStackTrace();
					System.out.println("Connection lost");
					ok = false;
				}
				if (str != null)
					retstr += str;
			} while (str != null && ok);
			return "";
		} else {
			System.out.println("No connection, nothing read");
			return null;
		}
	}

	/*public Message getMessage() {
		return null;
	}*/

	public String getDisplayContent() {
		JSONObject jmsg = new JSONObject("command", "request");
		jmsg.addItem(new JSONObject("parameter", "screen"));
		
		Message getdisp = new Message(jmsg.format(true));
		sendMessage(getdisp);
		String ret = readAll();
		
		return ret;
	}

	/*public void sendKey() {
		
	}*/
}
