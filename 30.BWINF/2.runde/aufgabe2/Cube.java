import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class Cube extends JTextArea {
    private static final long serialVersionUID = 1L;
    public static final int GERADEAUS_NORD_SUED = 0, GERADEAUS_OST_WEST = 1,
                            KNICK_NORD_OST = 2, KNICK_NORD_WEST = 3,
                            KNICK_OST_SUED = 4, KNICK_SUED_WEST = 5,
                            KREUZUNG = 6,
                            SACKGASSE_NORD = 7, SACKGASSE_OST   = 8,
                            SACKGASSE_SUED = 9, SACKGASSE_WEST  = 10,
                            TKREUZUNG_NORD = 11, TKREUZUNG_OST  = 12,
                            TKREUZUNG_SUED = 13, TKREUZUNG_WEST = 14;
    private DefaultListModel lm;
    private int type;

    public Cube(int t, int size, DefaultListModel m) {
        this.lm = m;
        this.type = t;

        setVisible(true);
        addMouseListener(new MouseListener() {
            public void mouseReleased(MouseEvent e) {}
            public void mousePressed(MouseEvent e) {}
            public void mouseExited(MouseEvent e) {}
            public void mouseEntered(MouseEvent e) {}
            public void mouseClicked(MouseEvent e) {
                if (e.getButton() == MouseEvent.BUTTON3
                    || e.getButton() == MouseEvent.BUTTON2)
                    lm.clear();

                if (e.getButton() == MouseEvent.BUTTON1) {
                    String eingabe;
                    eingabe = JOptionPane.showInputDialog("Anzahl:");

                    if (eingabe != null)
                        lm.insertElementAt(eingabe + " " + typeToString(), 0);
                }
            }
        });
    }

    private void paintBlock(Graphics g, int b, Color c) {
        g.setColor(c);
        int w = getWidth(),
            h = getHeight();

        switch(b) {
        case 1:
            g.fillRect(0, 0, w / 3, h / 3);
            break;

        case 2:
            g.fillRect(w / 3, 0, w / 3, h / 3);
            break;

        case 3:
            g.fillRect(2 * w / 3, 0, w / 3, h / 3);
            break;

        case 4:
            g.fillRect(0, h / 3, w / 3, h / 3);
            break;

        case 5:
            g.fillRect(w / 3, h / 3, w / 3, h / 3);
            break;

        case 6:
            g.fillRect(2 * w / 3, h / 3, w / 3, h / 3);
            break;

        case 7:
            g.fillRect(0, 2 * h / 3, w / 3, h / 3);
            break;

        case 8:
            g.fillRect(w / 3, 2 * h / 3, w / 3, h / 3);
            break;

        case 9:
            g.fillRect(2 * w / 3, 2 * h / 3, w / 3, h / 3);
            break;
        }
    }

    private void paintKreuzung(Graphics g) {
        paintBlock(g, 2, Color.yellow);
        paintBlock(g, 4, Color.yellow);
        paintBlock(g, 6, Color.yellow);
        paintBlock(g, 8, Color.yellow);
    }

    private void paintGeradeausNordSued(Graphics g) {
        paintBlock(g, 2, Color.yellow);
        paintBlock(g, 8, Color.yellow);
    }

    private void paintGeradeausOstWest(Graphics g) {
        paintBlock(g, 4, Color.yellow);
        paintBlock(g, 6, Color.yellow);
    }

    private void paintKnickNordOst(Graphics g) {
        paintBlock(g, 2, Color.yellow);
        paintBlock(g, 6, Color.yellow);
    }

    private void paintKnickNordWest(Graphics g) {
        paintBlock(g, 2, Color.yellow);
        paintBlock(g, 4, Color.yellow);
    }

    private void paintKnickOstSued(Graphics g) {
        paintBlock(g, 6, Color.yellow);
        paintBlock(g, 8, Color.yellow);
    }

    private void paintKnickSuedWest(Graphics g) {
        paintBlock(g, 4, Color.yellow);
        paintBlock(g, 8, Color.yellow);
    }

    private void paintTKreuzungNord(Graphics g) {
        paintBlock(g, 2, Color.yellow);
        paintBlock(g, 4, Color.yellow);
        paintBlock(g, 6, Color.yellow);
    }

    private void paintTKreuzungOst(Graphics g) {
        paintBlock(g, 2, Color.yellow);
        paintBlock(g, 6, Color.yellow);
        paintBlock(g, 8, Color.yellow);
    }

    private void paintTKreuzungSued(Graphics g) {
        paintBlock(g, 4, Color.yellow);
        paintBlock(g, 6, Color.yellow);
        paintBlock(g, 8, Color.yellow);
    }

    private void paintTKreuzungWest(Graphics g) {
        paintBlock(g, 2, Color.yellow);
        paintBlock(g, 4, Color.yellow);
        paintBlock(g, 8, Color.yellow);
    }

    private String typeToString() {
        switch (type) {
        case GERADEAUS_NORD_SUED:
            return "Geradeaus-Nord-Sued";

        case GERADEAUS_OST_WEST:
            return "Geradeaus-Ost-West";

        case KNICK_NORD_OST:
            return "Knick-Nord-Ost";

        case KNICK_NORD_WEST:
            return "Knick-Nord-West";

        case KNICK_OST_SUED:
            return "Knick-Ost-Sued";

        case KNICK_SUED_WEST:
            return "Knick-Sued-West";

        case KREUZUNG:
            return "Kreuzung";

        case SACKGASSE_NORD:
            return "Sackgasse-Nord";

        case SACKGASSE_OST:
            return "Sackgasse-Ost";

        case SACKGASSE_SUED:
            return "Sackgasse-Sued";

        case SACKGASSE_WEST:
            return "Sackgasse-West";

        case TKREUZUNG_NORD:
            return "T-Kreuzung-Nord";

        case TKREUZUNG_OST:
            return "T-Kreuzung-Ost";

        case TKREUZUNG_SUED:
            return "T-Kreuzung-Sued";

        case TKREUZUNG_WEST:
            return "T-Kreuzung-West";

        default:
            System.err.println("ERROR");
            System.exit(1);
        }

        return "ERROR";
    }

    public void paint(Graphics g) {
        g.setColor(Color.white);
        g.fillRect(0, 0, getWidth(), getHeight());

        for (int i = 1; i <= 9; i++)
            paintBlock(g, i, Color.gray);
        paintBlock(g, 5, Color.yellow);

        switch (type) {
        case GERADEAUS_NORD_SUED:
            paintGeradeausNordSued(g);
        break;

        case GERADEAUS_OST_WEST:
            paintGeradeausOstWest(g);
        break;

        case KNICK_NORD_OST:
            paintKnickNordOst(g);
        break;

        case KNICK_NORD_WEST:
            paintKnickNordWest(g);
        break;

        case KNICK_OST_SUED:
            paintKnickOstSued(g);
        break;

        case KNICK_SUED_WEST:
            paintKnickSuedWest(g);
        break;

        case KREUZUNG:
            paintKreuzung(g);
        break;

        case SACKGASSE_NORD:
            paintBlock(g, 2, Color.yellow);
        break;

        case SACKGASSE_OST:
            paintBlock(g, 6, Color.yellow);
        break;

        case SACKGASSE_SUED:
            paintBlock(g, 8, Color.yellow);
        break;

        case SACKGASSE_WEST:
            paintBlock(g, 4, Color.yellow);
        break;

        case TKREUZUNG_NORD:
            paintTKreuzungNord(g);
        break;

        case TKREUZUNG_OST:
            paintTKreuzungOst(g);
        break;

        case TKREUZUNG_SUED:
            paintTKreuzungSued(g);
        break;

        case TKREUZUNG_WEST:
            paintTKreuzungWest(g);
        break;

        default:
            System.err.println("ERROR");
            System.exit(1);
        }
    }
}
