package conn;

/**
 * Message contains "8_chrlen + \n + message"
 * 
 * @author Jani Koskinen
 *
 */
public class Message {

	private String data;

	public Message(String msg) {
		data = msg;
	}

	public String toSendFormat() {
		return String.format("%.8x\n%s", data.length(), data);
	}
}
