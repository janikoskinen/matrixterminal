package ui;

import java.awt.GridLayout;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JPanel;

public class NumericPad extends JPanel {
	private JButton buttons[];

	public final String labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "#"};

	NumericPad(int gap) {
		this(gap, gap);
	}

	public NumericPad(int vgap, int hgap) {
		buttons = new JButton[12];
		for (int i = 0 ; i < 12 ; i++) {
			buttons[i] = new JButton(labels[i]);
			buttons[i].setActionCommand(labels[i]);
		}

		setLayout(new GridLayout(4, 3, vgap, hgap));
		for (int i = 1 ; i < 10 ; i++)
			add(buttons[i]);
		add(buttons[10]);
		add(buttons[0]);
		add(buttons[11]);
	}

	void setActionListener(ActionListener listener) {
		for (int i = 0 ; i < 12 ; i++)
			buttons[i].addActionListener(listener);
	}
}
