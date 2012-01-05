import java.awt.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class GUI extends JFrame {
    static final long serialVersionUID = 1;
    private JPanel chooser;
    private JList list;
    private DefaultListModel lm;
    private Button button;

    public GUI() {
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setBounds(100, 100, 623, 433);
        //setLayout(new GridLayout(1, 2, 10, 0));
        setLayout(null);
        setTitle("Auswahl der Wuerfel");

        chooser = new JPanel();
        chooser.setLayout(new GridLayout(4, 4, 20, 20));

        button = new Button("Speichern");
        button.addMouseListener(new MouseListener() {
            public void mouseReleased(MouseEvent e) {}
            public void mousePressed(MouseEvent e) {}
            public void mouseExited(MouseEvent e) {}
            public void mouseEntered(MouseEvent e) {}
            public void mouseClicked(MouseEvent e) {
                try {
                    if (e.getButton() == MouseEvent.BUTTON1) {
                        File f;
                        JFileChooser fc;
                        FileWriter w = null;

                        if (lm.size() == 0)
                            return;

                        fc = new JFileChooser(".");
                        fc.showSaveDialog(null);
                        f = fc.getSelectedFile();

                        if (f == null)
                            return;

                        w = new FileWriter(f);

                        while (lm.size() > 0)
                            w.write(lm.remove(0).toString() + "\n");

                        w.close();
                    }
                } catch (IOException ex) {
                    System.err.println(ex);
                    System.exit(1);
                }
            }
        });

        lm   = new DefaultListModel();
        list = new JList(lm);
        JScrollPane p = new JScrollPane(list);
        list.setLayoutOrientation(JList.VERTICAL);

        for (int i = 0; i < 15; i++)
            chooser.add(new Cube(i, 100, lm));
        chooser.add(button);

        p.setBounds(5, 5, 200, 400);
        chooser.setBounds(215, 5, 400, 400);

        add(p);
        add(chooser);

        list.setVisible(true);
        chooser.setVisible(true);

        setResizable(false);
        setVisible(true);
    }
}
