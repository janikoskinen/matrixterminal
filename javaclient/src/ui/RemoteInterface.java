package ui;

import java.awt.Font;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SpringLayout;

import conn.Message;
import conn.TerminalInterface;


public class RemoteInterface extends JFrame implements ActionListener {

	private SpringLayout layout;

	private JTextArea txtDisplay;
	private NumericPad btnNumpad;
	private JButton btnRefresh;

	private TerminalInterface terminal;

	public RemoteInterface() {
		layout = new SpringLayout();
		setLayout(layout);

		txtDisplay = new JTextArea();
		txtDisplay.setSize(130, 40);
		//display.setFont();

		btnNumpad = new NumericPad(10);
		btnNumpad.setActionListener(this);
		btnNumpad.setSize(200, 240);

		btnRefresh = new JButton("Refresh");
		btnRefresh.addActionListener(this);
		btnRefresh.setActionCommand("refresh");

		add(txtDisplay);
		add(btnNumpad);
		add(btnRefresh);

		layout.putConstraint(SpringLayout.NORTH, txtDisplay, 10, SpringLayout.NORTH, this);
		layout.putConstraint(SpringLayout.HORIZONTAL_CENTER, txtDisplay, 10, SpringLayout.HORIZONTAL_CENTER, this);
		layout.putConstraint(SpringLayout.NORTH, btnNumpad, 10, SpringLayout.SOUTH, txtDisplay);

		setVisible(true);
		setSize(300,500);

		addWindowListener(new WindowAdapter(){
			public void windowClosing(WindowEvent e){
				System.exit(0);
			}
		});

		terminal = new TerminalInterface();

	}


	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("refresh")) {
			terminal.connectTo("localhost", 6100);
			terminal.sendMessage(new Message("Refresh needed"));
		}
	}
}
