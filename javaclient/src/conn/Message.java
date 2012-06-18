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
		if (data != null) {
			Object args[] = {new Integer(data.length()), data};
			return String.format("%08x\n%s", args);
			//return String.format("%08x\n%s", data.length(), data);
		} else {
			return "";
		}
	}
}
