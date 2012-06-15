package ui;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SpringLayout;
import javax.swing.border.Border;

import conn.Message;
import conn.TerminalInterface;


public class RemoteInterface extends JFrame implements ActionListener {

	private final String StatusStr = "Status: ";

	private SpringLayout layout;

	private JTextArea txtDisplay;
	private NumericPad btnNumpad;
	private JButton btnRefresh;
	private JLabel lblStatus;

	private TerminalInterface terminal;

	private String clearDispContent;

	public RemoteInterface() {
		layout = new SpringLayout();
		setLayout(layout);

		clearDispContent = "                    \n                    \n                    \n                    ";

		txtDisplay = new JTextArea();
		//txtDisplay.setSize(130, 40);
		txtDisplay.setEnabled(false);
		txtDisplay.setFont(new Font("Courier", Font.PLAIN, 12));
		txtDisplay.setText(clearDispContent);
		txtDisplay.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		lblStatus = new JLabel(StatusStr + "not connected");

		btnNumpad = new NumericPad(10);
		btnNumpad.setActionListener(this);
		btnNumpad.setSize(200, 240);

		btnRefresh = new JButton("Refresh");
		btnRefresh.addActionListener(this);
		btnRefresh.setActionCommand("refresh");

		add(txtDisplay);
		add(btnNumpad);
		add(btnRefresh);

		layout.putConstraint(SpringLayout.NORTH, txtDisplay, 100, SpringLayout.NORTH, this);
		layout.putConstraint(SpringLayout.WEST, txtDisplay, 20, SpringLayout.WEST, this);
		layout.putConstraint(SpringLayout.NORTH, btnNumpad, 10, SpringLayout.SOUTH, txtDisplay);
		layout.putConstraint(SpringLayout.HORIZONTAL_CENTER, btnNumpad, 0, SpringLayout.HORIZONTAL_CENTER, txtDisplay);
		setVisible(true);
		setSize(300,500);

		addWindowListener(new WindowAdapter(){
			public void windowClosing(WindowEvent e){
				System.exit(0);
			}
		});

		terminal = new TerminalInterface("192.168.1.2", 6100);

	}


	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("refresh")) {
			String content = terminal.getDisplayContent();
			if (content != null) {
				txtDisplay.setText(content);
			} else {
				txtDisplay.setText(clearDispContent);
			}
			System.out.println("Disp:\n"+content);
		}
	}
}
