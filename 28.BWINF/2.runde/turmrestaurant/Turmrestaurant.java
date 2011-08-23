/*
  Klasse: Turmrestaurant
          -> zeigt das Turmrestaurant in einer GUI und beliefert die Funktion
             setzeGruppe entweder mit zufaelligen Gruppen
             oder aber mit den Gruppen der Schueler
             
  enthaltenen Klassen: - ButtonListener
                       - SliderListener
                       - Zeichnen
                       
  Methoden: - initButtons
            - initSlider
            - initLabels
            - slider_auslesen
            - GUI
            - freieSitzplaetze
            - setzeRechtsVonGruppe
            - setzeLinksVonGruppe
            - sortiereNachStartIndex
            - sortiereNachGaeste
            - freiePlaetzeListeErstellen
            - setzeGruppeVonBis
            - platziereGruppeAmAnfang
            - getGruppeRechtsVon
            - getGruppeLinksVon
            - setzeGruppe
            - geschaetzt
*/

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.awt.Font;
import java.awt.Color;
import java.awt.GridLayout;
import java.awt.Graphics;
import java.awt.BorderLayout;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.util.List;
import java.util.ArrayList;

class Turmrestaurant {
    //Konstanten
    private final int MAX_SITZPLAETZE = 55;
    
    //JFrames
    private JFrame restaurant, //hier wird das Turmrestaurant angezeigt
                   optionen; //hier koennen "Parameter" eingestellt werden 
    
    //JSlider
    private JSlider slider1, //Sitzplaetze
                    slider2, //Simulationsgeschwindigkeit
                    slider3, //maximale Anzahl Gruppen pro Tick
                    slider4, //maximale Anzahl Gaeste pro Gruppe
                    slider5, //maximale Verweildauer
                    slider6; //Erfahrung des Obers
                    
    //JTextArea zur Anzeige eines Textes, wenn sich der Ober aergert
    JTextArea textarea;
    
    //JLabels zur Beschriftung der Slider
    private JLabel label1, label2, label3,
                   label4, label5, label6;
    
    //JButtons
    private JButton button1, //Starten der Simulation
                    button2, //den Ober einen Streich spielen
                    button3; //Stoppen der Simulation
    
    //Panel auf dem das Turmrestaurant gezeichnet wird
    private Zeichnen zpanel;
    
    //ArrayListen
    private List<Gruppe> gruppen,      //Gruppen die im Turmrestaurant sind
                         gruppen_ober, //"Kopie" von gruppen
                                       //(mit "abgeschaetzten" Restzeiten)
                         freiePlaetze; //Plaetze, wo keiner sitzt
    private List<JLabel> rest; //JLabels zur Anzeige der Restzeit
    
    private int  n, //Anzahl Sitzplaetze 
                 geschwindigkeit, //Simulations-Geschwindigkeit
                 max_gruppen_pro_tick, //maximale Gruppenanzahl pro Tick
                 max_gaeste_pro_gruppe, //maximale Anzahl von
                                        //Gaesten pro Gruppe
                 max_dauer, //maximale Verweildauer der Gruppen
                 erfahrung; //Erfahrung des Ober
    private long wait; //Millisekunden zwischen 2 Ticks
    
    //Listener
    ButtonListener bl = new ButtonListener(); //fuer die JButtons
    SliderListener sl = new SliderListener(); //fuer die JSlider
    
    //Konstruktor
    public Turmrestaurant() {
        this.GUI();
    }
    
    //Listener:
    /*
      ActionListener fuer die JButtons
    */
    class ButtonListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            //Quelle des ActionEvents herausfinden
            JButton source = (JButton) e.getSource();
            
            //herausfinden welcher Button gedrueckt wurde
            if(source == button1) { //Start-Button wurde gedrueckt
                //nur Stop-Button kann gedrueckt werden 
                button1.setEnabled(false);
                button2.setEnabled(false);
                button3.setEnabled(true);
                
                //Textfarbe des Label1 grau setzen
                label1.setForeground(Color.gray);
                
                //Sliderwert fuer Anzahl der Sitzplaetze
                //darf nicht veraendert werden
                slider1.setEnabled(false);
                
                //neuen Thread erstellen
                SimulationThread simulation =  new SimulationThread();
                
                //Thread/Simualtion starten
                simulation.start();
            } else if(source == button2) { //Streich-Button wurde gedrueckt
                //nur Stop-Button kann gedrueckt werden
                button1.setEnabled(false);
                button2.setEnabled(false);
                button3.setEnabled(true);
                
                //Textfarbe der Labels auf grau setzen
                label1.setForeground(Color.gray);
                label3.setForeground(Color.gray);
                label4.setForeground(Color.gray);
                label5.setForeground(Color.gray);
                
                //nur noch 2 Sliderwerte duerfen veraendert werden
                //(Simulationsgeschwindigkeit und Erfahrung des Obers)
                slider1.setEnabled(false);
                slider3.setEnabled(false);
                slider4.setEnabled(false);
                slider5.setEnabled(false);
                
                //neuen Thread erstellen
                StreichThread streich = new StreichThread();
                
                //Thread/Streich starten
                streich.start();
            } else if(source == button3) { //Stop-Button wurde gedrueckt
                //nur Start-Buttons koennen gedrueckt werden
                button1.setEnabled(true);
                button2.setEnabled(true);
                button3.setEnabled(false);
                
                //Textfarbe der Labels auf schwarz setzen
                label1.setForeground(Color.black);
                label3.setForeground(Color.black);
                label4.setForeground(Color.black);
                label5.setForeground(Color.black);
                
                //alle Slider-Werte duerfen veraendert werden
                slider1.setEnabled(true);
                slider3.setEnabled(true);
                slider4.setEnabled(true);
                slider5.setEnabled(true);
                
                //aufraeumen
                System.gc();
            }
        }
    }
    
    /* 
      Listener fuer die JSlider
    */
    class SliderListener implements ChangeListener {
        public void stateChanged(ChangeEvent e) {
            //Quelle des ChangeEvents herausfinden
            JSlider source = (JSlider) e.getSource();
            
            //herausfinden welcher Sliderwert veraendert wurde:
            if(source == slider1) { //Slider fuer Anzahl Sitzplaetze
                //Anzahl der Sitzplaetze auslesen
                n = source.getValue();
                
                //Anzahl der Sitzplaetze im Titel des JFrames anzeigen
                restaurant.setTitle("Turmrestaurant mit " + n +
                                    " Sitzplaetzen");
                
                //Sitzplaetze neu zeichnen
                restaurant.repaint();
            } else if(source == slider2) {
                //Slider fuer Simulationsgeschwindigkeit
                
                //Simulationsgeschwindigkeit auslesen
                geschwindigkeit = source.getValue();
                
                //Wartezeit zwischen den Ticks berechnen
                if(geschwindigkeit == 0)
                    wait = Long.MAX_VALUE;
                else
                    wait = (long) ((60.0 / (double) geschwindigkeit) * 1000.0);
                
                //Labeltext aktualisieren
                label2.setText(geschwindigkeit + " Ticks pro Minute");
            } else if(source == slider3) {
                //Slider fuer max. Anzahl Gruppen pro Tick
                
                //maximale Anzahl Gruppen pro Tick auslesen
                max_gruppen_pro_tick = source.getValue();
            } else if(source == slider4) {
                //Slider fuer max. Anzahl Gaeste pro Gruppen 
                
                //maximale Anzahl Gaeste pro Gruppe auslesen
                max_gaeste_pro_gruppe = source.getValue();
            } else if(source == slider5) {
                //Slider fuer max. Verweildauer der Gruppen
                
                //maximale Verweildauer auslesen
                max_dauer = source.getValue();
                
                //Labeltext aktualisieren
                label5.setText("Maximale Verweildauer der Gruppen (" +
                               max_dauer + " Ticks)");
            } else if(source == slider6) { //Slider fuer Erfahrung des Obers
                //Erfahrung des Obers auslesen
                erfahrung = source.getValue();
                
                //Labeltext aktualisieren
                label6.setText("Erfahrung des Obers (" + erfahrung + " %)");
            }
        }
    }
    
    /*
      Methode zum initialisieren der JButtons.
    */
    private void initButtons() {
        //Objekte anlegen und Button-Texte festlegen
        button1 = new JButton("Start");
        button2 = new JButton("Streich spielen");
        button3 = new JButton("Stopp");
        
        //ActionListener festlegen
        button1.addActionListener(bl);
        button2.addActionListener(bl);
        button3.addActionListener(bl);
        
        //Stop-Button kann nicht geklickt werden
        button3.setEnabled(false);
    }
    
    /*
      Methode zum initialisieren der JSlider.
    */
    private void initSlider() {
        //Objekte anlegen und Start-, End- und Initialwerte festlegen
        slider1 = new JSlider(JSlider.HORIZONTAL, 25, MAX_SITZPLAETZE, n);
        slider2 = new JSlider(JSlider.HORIZONTAL, 0, 600, geschwindigkeit);
        slider3 = new JSlider(JSlider.HORIZONTAL, 0, 10,
                              max_gruppen_pro_tick);
        slider4 = new JSlider(JSlider.HORIZONTAL, 2, 20,
                              max_gaeste_pro_gruppe);
        slider5 = new JSlider(JSlider.HORIZONTAL, 1, 400, max_dauer);
        slider6 = new JSlider(JSlider.HORIZONTAL, 0, 100, erfahrung);
        
        //ChangeListener festlegen
        slider1.addChangeListener(sl);
        slider2.addChangeListener(sl);
        slider3.addChangeListener(sl);
        slider4.addChangeListener(sl);
        slider5.addChangeListener(sl);
        slider6.addChangeListener(sl);
        
        //TickSpacings festlegen
        slider1.setMajorTickSpacing(2);
        slider1.setMinorTickSpacing(1);
        slider2.setMajorTickSpacing(100);
        slider2.setMinorTickSpacing(1);
        slider3.setMajorTickSpacing(1);
        slider3.setMinorTickSpacing(1);
        slider4.setMajorTickSpacing(2);
        slider4.setMinorTickSpacing(1);
        slider5.setMajorTickSpacing(133);
        slider5.setMinorTickSpacing(1);
        slider6.setMajorTickSpacing(10);
        slider6.setMinorTickSpacing(1);
        
        //Rahmen festlegen
        slider1.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));
        slider2.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));
        slider3.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));
        slider4.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));
        slider5.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));
        slider6.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 0));
        
        //sonstige Eigenschaften festlegen
        slider1.setPaintTicks(true);
        slider1.setPaintLabels(true);
        slider1.setSnapToTicks(true);
        slider2.setPaintTicks(false);
        slider2.setPaintLabels(true);
        slider2.setSnapToTicks(true);
        slider3.setPaintTicks(true);
        slider3.setPaintLabels(true);
        slider3.setSnapToTicks(true);
        slider4.setPaintTicks(true);
        slider4.setPaintLabels(true);
        slider4.setSnapToTicks(true);
        slider5.setPaintTicks(false);
        slider5.setPaintLabels(true);
        slider5.setSnapToTicks(true);
        slider6.setPaintTicks(false);
        slider6.setPaintLabels(true);
        slider6.setSnapToTicks(true);
    }
    
    /*
      Methode zum initialisieren der Labels.
    */
    private void initLabels() {
        //Labels zur Beschriftung der JSlider
        //Objekte anlegen und Labeltexte festlegen
        label1 = new JLabel("Anzahl Sitzplaetze", SwingConstants.CENTER);
        label2 = new JLabel(geschwindigkeit + " Ticks pro Minute",
                            SwingConstants.CENTER);
        label3 = new JLabel("Maximale Anzahl Gruppen pro Tick", 
                            SwingConstants.CENTER);
        label4 = new JLabel("Maximale Anzahl Gaeste pro Gruppe",
                            SwingConstants.CENTER);
        label5 = new JLabel("Maximale Verweildauer der Gruppen (" +
                            max_dauer + " Ticks)", SwingConstants.CENTER);
        label6 = new JLabel("Erfahrung des Obers (" + erfahrung + " %)",
                            SwingConstants.CENTER);
        
        //Labels zur Anzeige der Restzeit:
        //Arraylist anlegen
        rest = new ArrayList<JLabel>();
        
        //der ArrayList JLabel-Objekte hinzufuegen
        for(int i = 0; i < MAX_SITZPLAETZE; i++) {
            JLabel temp = new JLabel();
            temp.setVisible(false); //JLabel ist nicht sichtbar
            rest.add(temp);
        }
    }
    
    /*
      Methode zum Auslesen aller Parameter der Simulation.
    */
    private void slider_auslesen() {
        //Anzahl der Sitzplaetze auslesen
        n = slider1.getValue();
        
        //Simulationsgeschwindigkeit auslesen
        geschwindigkeit = slider2.getValue();
        
        //maximale Anzahl Gruppen pro Tick auslesen
        max_gruppen_pro_tick = slider3.getValue();
        
        //maximale Anzahl Gaeste pro Gruppe auslesen
        max_gaeste_pro_gruppe = slider4.getValue();
        
        //maximale Verweildauer auslesen
        max_dauer = slider5.getValue();
        
        //Erfahrung des Obers auslesen
        erfahrung = slider6.getValue();
    }
    
    /*
      Methode zum initialisieren der GUI.
    */
    private void GUI() {
        //ArrayListen fuer die Gruppen anlegen
        gruppen = new ArrayList<Gruppe>();
        gruppen_ober = new ArrayList<Gruppe>();
        freiePlaetze = new ArrayList<Gruppe>();
        
        //Initialwerte
        n = 30;
        geschwindigkeit = 60;
        max_gruppen_pro_tick = 1;
        max_gaeste_pro_gruppe = 6;
        max_dauer = 60;
        erfahrung = 80;
        
        //GUI - Komponenten initialisieren:
        
        //JFrame-Objekte erstellen und Titel festlegen
        restaurant = new JFrame("Turmrestaurant");
        optionen = new JFrame("Optionen");
        
        //Layout des Optionen-Frames festlegen
        optionen.setLayout(new GridLayout(15, 0, 0, 0));
        
        //Schliessen der Frames moeglich machen
        restaurant.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        optionen.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        //Groesse und Position der JFrames festlegen
        restaurant.setBounds(500, 0, 600, 600);
        optionen.setBounds(0, 0, 500, 703);
        
        //Framegroesse kann veraendert werden
        restaurant.setResizable(true);
        optionen.setResizable(true);
        
        //JButtons initialisieren
        initButtons();
        
        //JSlider initialisieren 
        initSlider();
        
        //Startwerte auslesen
        slider_auslesen();
        
        //Wartezeit zwischen den einzelnen Threads bestimmen
        wait = (long) (60.0 / (double) geschwindigkeit * 1000.0);
        
        //JLabels initialisieren
        initLabels();
        
        //Zeichenpanel-Objekt anlegen
        zpanel = new Zeichnen();
        
        //JLabels zur Anzeige der Restzeit dem Zeichenpanel hinzufuegen
        for(int i = 0; i < MAX_SITZPLAETZE; i++)
            zpanel.add(rest.get(i));
        
        //TextArea-Objekt anlegen
        textarea = new JTextArea();
        
        //Text zentrieren
        textarea.setAlignmentY(java.awt.Component.CENTER_ALIGNMENT);
        textarea.setAlignmentX(java.awt.Component.CENTER_ALIGNMENT);
        
        //Textfarbe auf rot setzen
        textarea.setForeground(Color.red);
        
        //Text in der TextArea soll nicht veraenderbar sein
        textarea.setEditable(false);
        
        //die TextArea dem Zeichenpanel hinzufuegen
        zpanel.add(textarea);
        
        //Zeichenpanel dem Turmrestaurant-Frame hinzufuegen
        restaurant.getContentPane().add(BorderLayout.CENTER, zpanel);
        
        //Anzahl der Sitzplaetze im Titel des JFrames anzeigen
        restaurant.setTitle("Turmrestaurant mit " + n + " Sitzplaetzen");
        
        //dem Optionen-Frame alle benoetigten Komponenten hinzufuegen
        optionen.add(label1);
        optionen.add(slider1);
        optionen.add(label2);
        optionen.add(slider2);
        optionen.add(label3);
        optionen.add(slider3);
        optionen.add(label4);
        optionen.add(slider4);
        optionen.add(label5);
        optionen.add(slider5);
        optionen.add(label6);
        optionen.add(slider6);
        optionen.add(button1);
        optionen.add(button2);
        optionen.add(button3);
        
        //Frames zeigen
        restaurant.setVisible(true);
        optionen.setVisible(true);
    }
    
    /*
      Methode zaehlt die freien Sitzplaetze im Turmrestaurant.
      und gibt dies zurueck
    */
    private int freieSitzplaetze() {
        int count = 0;
        
        //Gaeste im Turmrestaurant zaehlen
        for(int i = 0; i < gruppen.size(); i++)
            count += gruppen.get(i).getAnzahlGaeste();
            
        //Anzahl der Gaeste von Anzahl der Sitzplaetze abziehen
        return (n - count); //Anzahl der freien Sitzplaetze zurueckgeben
    }
    
    /* 
      Methode setzt eine Gruppe rechts neben einer anderen.
      
      benoetigte Parameter:
        gr - Gruppe die zu platzieren ist
         d - Gruppe neben der gr platziert werden soll
    */
    private void setzeRechtsVonGruppe(Gruppe gr, Gruppe d) {
        int start, //StartIndex
            ende;  //EndIndex
        
        //StartIndex festlegen
        start = d.getEndeIndex() + 1;
        
        //EndIndex festlegen
        ende = start + gr.getAnzahlGaeste() - 1;
        
        //Start- und EndIndex der Gruppe festlegen
        gr.setStartIndex((start >= n) ? (start - n) : (start));
        gr.setEndeIndex((ende >= n) ? (ende - n) : (ende));
        
        //Gruppe wrid "platziert"
        gruppen.add(gr);
        gruppen_ober.add(geschaetzt(gr));
    }
    
    /* 
      Methode setzt eine Gruppe links neben einer anderen.
      
      benoetigte Parameter:
        gr - Gruppe die zu platzieren ist
         d - Gruppe neben der gr platziert werden soll
    */
    private void setzeLinksVonGruppe(Gruppe gr, Gruppe d) {
        int start, //StartIndex
            ende;  //EndIndex
            
        //StartIndex festlegen
        start = d.getStartIndex() - 1;
        
        //EndIndex festlegen
        ende = start - (gr.getAnzahlGaeste() - 1);
        
        //Start- und EndIndex der Gruppe festlegen 
        gr.setStartIndex((ende < 0) ? (n + ende) : (ende));
        gr.setEndeIndex((start < 0) ? (n + start) : (start));
        
        //Gruppen wird "platziert"
        gruppen.add(gr);
        gruppen_ober.add(geschaetzt(gr));
    }

    /*
      Methode sortiert die ArrayListe gruppen_ober
      nach den StartIndizies (aufwaerts).
    */
    private void sortiereNachStartIndex() {
        //Hilfsvariablen:
        Gruppe temp;
        int merk;
        
        //alle Gruppen durchgehen
        for(int i = 0; i < gruppen_ober.size(); i++) {
            merk = 0;
           
            //Gruppe mit kleinsten StartIndex herausfinden
            //(von den noch nicht sortierten Gruppen)
            for(int j = 1; j < gruppen_ober.size() - i; j++)
                if(gruppen_ober.get(j).getStartIndex()
                   < gruppen_ober.get(merk).getStartIndex())
                    merk = j;
            
            //Gruppe aus ArrayListe entfernen
            temp = gruppen_ober.remove(merk);
            
            //Gruppe hinten an ArrayListe einfuegen
            gruppen_ober.add(temp);
        }
    }
    
    /*
      Methode sortiert die ArrayListe freiePlaetze
      nach der Anzahl der freien Plaetze (aufwaerts).
    */
    private void sortiereNachGaeste() {
        //Hilfsvariablen
        Gruppe temp;
        int merk;
        
        for(int i = 0; i < freiePlaetze.size(); i++) {
            merk = 0;
            
            //Gruppe mit kleinster GaesteAnzahl herausfinden
            //(von den noch nicht sortierten Gruppen)
            for(int j = 1; j < freiePlaetze.size() - i; j++)
                if(freiePlaetze.get(j).getAnzahlGaeste()
                   < freiePlaetze.get(merk).getAnzahlGaeste())
                    merk = j;
            
            //Gruppe aus ArrayListe entfernen
            temp = freiePlaetze.remove(merk);
            
            //Gruppe hinten an ArrayListe einfuegen
            freiePlaetze.add(temp);
        }
    }
    
    /*
      Methode erstellt eine ArrayListe von freien Plaetzen im Turmrestaurant.
      Dabei wird die Liste gruppen_ober benutzt und veraendert (sortiert).
    */
    private void freiePlaetzeListeErstellen() {
        //Hilfsvariablen:
        int count, start, ende;
        Gruppe temp;
        
        //ArrayListe leeren
        freiePlaetze.clear();
        
        //ArrayListe gruppen_ober sortieren (nach StartIndex - aufwaerts)
        sortiereNachStartIndex();
        
        for(int i = 0; i < gruppen_ober.size(); i++) {
            if(i == gruppen_ober.size() - 1) { //letzter Durchlauf
              //freie Plaetze zwischen letzter und erster Gruppe herausfinden
              start = gruppen_ober.get(gruppen.size() - 1).getEndeIndex() + 1;
              ende = gruppen_ober.get(0).getStartIndex() - 1;
            } else {
              //freie Plaetze zwischen 2 benachbarten Gruppen herausfinden
              start = gruppen_ober.get(i).getEndeIndex() + 1;
              ende = gruppen_ober.get(i + 1).getStartIndex() - 1;
            }
            
            //Werte anpassen
            if(start >= n)
                start = start - n;
            if(ende < 0)
                ende = n + ende;
            
            //Anzahl der freien Plaetze ermitteln
            if(start > ende)
                count = (n - (start - ende)) + 1;
            else
                count = (ende - start) + 1;
            
            if(count == n)
                continue; //keine freien Plaetze zwischen den beiden Gruppen
            
            //neue Gruppe erstellen
            temp = new Gruppe();
            
            //Eigenschaften festlegen
            temp.setStartIndex(start);
            temp.setEndeIndex(ende);
            temp.setDauer(0);
            temp.setRestzeit(0);
            temp.setAnzahlGaeste(count);
            
            //Gruppe der freiePlaetze-Liste hinzufuegen
            freiePlaetze.add(temp);
        }
    }
    
    /*
      Methode setzt eine Gruppe von einem StartIndex bis zu einem EndIndex.
      
      benoetigte Parameter:
        gr - Gruppe die platziert werden soll
        start - StartIndex
        ende - EndIndex
    */
    private void setzeGruppeVonBis(Gruppe gr, int start, int ende) {
        //EndIndex anpassen
        if(start > ende)
            ende = ende + n;
        
        //Start- und EndIndex festlegen
        gr.setStartIndex(start);
        gr.setEndeIndex((ende >= n) ? (ende - n) : (ende));
        
        //Gruppe wird "platziert"
        gruppen.add(gr);
        gruppen_ober.add(geschaetzt(gr));
    }
    
    /*
      Methode platziert eine Gruppe, wenn das Turmrestaurant leer ist.
    */
    private void platziereGruppeAmAnfang(Gruppe gr) {
        //Start- und EndIndex festlegen
        gr.setStartIndex(0);
        gr.setEndeIndex(gr.getAnzahlGaeste() - 1);
        
        //Gruppe wird "platziert"
        gruppen.add(gr);
        gruppen_ober.add(geschaetzt(gr));
    }
    
    /*
      Methode liefert die Gruppe,
      die rechts von einem freien Bereich platziert ist.
      
      benoetigte Parameter:
        frei - freier Bereich (als Gruppe)
        
      Rueckgabewerte:
        null, wenn keine Gruppe rechts neben dem freien Bereich platziert ist,
        sonst die Gruppe, die rechts neben dem freien Bereich platziert ist.
    */
    private Gruppe getGruppeRechtsVon(Gruppe frei) {
        int start; //StartIndex
        
        //StartIndex einer (moeglichen) Gruppe
        start = frei.getEndeIndex() + 1;
        
        //Wert anpassen
        if(start >= n)
            start = start - n;
        
        //alle Gruppen durchgehen
        for(int i = 0; i < gruppen_ober.size(); i++)
            //pruefen, ob Gruppe rechts neben dem freien Platz platziert ist
            if(gruppen_ober.get(i).getStartIndex() == start)
                //Gruppe zurueckgeben,
                //die rechts neben den freien Bereich platziert ist
                return gruppen_ober.get(i);
        
        //keine Gruppe ist rechts neben dem freien Bereich platziert
        return null;
    }
    
    /*
      Methode liefert die Gruppe,
      die links von einem freien Bereich platziert ist.
      
      benoetigte Parameter:
        frei - freier Bereich (als Gruppe)
        
      Rueckgabewerte:
        null, wenn keine Gruppe links neben dem freien Bereich platziert ist,
        sonst die Gruppe, die links neben dem freien Bereich paltziert ist.
    */
    private Gruppe getGruppeLinksVon(Gruppe frei) {
        int ende; //EndIndex
        
        //EndIndex einer (moeglichen) Gruppe
        ende = frei.getStartIndex() - 1;
        
        //Wert anpassen
        if(ende < 0)
            ende = n + ende;
        
        //alle Gruppen durchgehen
        for(int i = 0; i < gruppen_ober.size(); i++)
            //pruefen, ob Gruppe links neben dem freien Bereich platziert ist
            if(gruppen_ober.get(i).getEndeIndex() == ende)
                //Gruppen zurueckgeben,
                //die links neben den freien Bereich platziert ist
                return gruppen_ober.get(i);
        
        //keine Gruppe ist links neben dem freien Bereich platziert
        return null;
    }
    
    /*
      Methode setzt eine Gruppe nach bestimmten Regeln,
      damit der Ober sich moeglichst selten aergert.
      
      benoetigte Parameter:
        gr - Gruppe die platziert werden soll
        
      Rueckgabewerte
        0: Gruppe konnte nicht platziert werden
        1: Gruppe konnte platziert werden
        2: nicht genuegend freie Sitzplaetze fuer Gruppe
    */
    private int platziereGruppe(Gruppe gr) {
        if(freieSitzplaetze() < gr.getAnzahlGaeste())
            return 2; //nicht genuegend freie Plaetze fuer Gruppe
        
        if(freieSitzplaetze() == n) {
            //alle Sitzplaetze frei:
            
            //Gruppe platzieren
            platziereGruppeAmAnfang(gr);
            
            //Gruppe konnte gesetzt werden
            return 1;
        } else { 
            //nicht alle Sitzplaetze frei:
            
            //freie Plaetze als Gruppen in eine ArrayList einfuegen
            freiePlaetzeListeErstellen();
            
            if(freiePlaetze.size() == 1) {
                //nur ein zusammenhaengender Bereich von freien Plaetzen:
                
                if(gruppen_ober.size() == 1) {
                    //nur eine Gruppe im Turmrestaurant:
                    
                    //Gruppe rechts neben der
                    //Gruppe im Turmrestaurant platzieren
                    setzeRechtsVonGruppe(gr, gruppen_ober.get(0));

                    //Gruppe wurde platziert
                    return 1;
                } else {
                    //mehrere Gruppen im Turmrestaurant:
                    
                    int r1, r2; //Hilfsvariablen
                    Gruppe links, rechts; //Gruppen links und rechts
                                          //vom freien Bereich
                        
                    links = getGruppeLinksVon(freiePlaetze.get(0));
                    rechts = getGruppeRechtsVon(freiePlaetze.get(0));
                    
                    //Restzeiten auslesen
                    r1 = links.getRestzeit();
                    r2 = rechts.getRestzeit();
                    
                    //neben Gruppe setzen, die laenger bleibt 
                    if(r1 > r2)
                        setzeRechtsVonGruppe(gr, links);
                    else
                        setzeLinksVonGruppe(gr, rechts);
                    
                    //Gruppe wurde platziert
                    return 1;
                }
            } else {
                //mehrere freie Bereiche:
                
                //freiePlaetze-Liste durchgehen
                for(int i = 0; i < freiePlaetze.size(); i++) {
                    //Gruppe passt genau in eine "Luecke"
                    if(freiePlaetze.get(i).getAnzahlGaeste()
                       == gr.getAnzahlGaeste()) {
                        setzeGruppeVonBis(gr,
                                          freiePlaetze.get(i).getStartIndex(),
                                          freiePlaetze.get(i).getEndeIndex());

                        //Gruppe wurde platziert
                        return 1;
                    }
                }
                
                //guten Platz suchen:
                
                //sortieren der freiePlaetze-Liste
                //nach Anzahl der freien Plaetze (GaesteAnzahl)
                sortiereNachGaeste();
                
                //freiePlaetze-Liste durchgehen
                for(int i = 0; i < freiePlaetze.size(); i++) {
                    //Gruppe platzieren, wenn genug Plaetze frei sind
                    if(gr.getAnzahlGaeste()
                       < freiePlaetze.get(i).getAnzahlGaeste()) {
                        Gruppe links,  //Gruppe links von den freien Plaetzen
                               rechts; //Gruppe rechts von den freien Plaetzen
                        int r1, r2; //Hilfsvariablen
                        
                        //Gruppen ermitteln
                        links = getGruppeLinksVon(freiePlaetze.get(i));
                        rechts = getGruppeRechtsVon(freiePlaetze.get(i));
                        
                        //Restzeiten auslesen
                        r1 = links.getRestzeit();
                        r2 = rechts.getRestzeit();
                        
                        //links oder rechts platzieren
                        //(neben der Gruppe, die laenger bleibt):
                        if(r1 > r2)
                            setzeRechtsVonGruppe(gr, links);
                        else
                            setzeLinksVonGruppe(gr, rechts);
                        
                        //Gruppe wurde platziert
                        return 1;
                    }
                }
            }
        }
        
        //Gruppe konnte nicht platziert werden
        return 0;
    }
    
    /*
      Methode liefert eine Gruppe,
      die eine vom Ober "geschaetzte" Restzeit enthaelt.
      Bei der Berechnung wird die Variable erfahrung verwendet.
      
      benoetigte Parameter:
        gr - Gruppe, deren Restzeit vom Ober "geschaetzt" werden soll
        
      Rueckgabewerte:
        Eine Gruppe, die eine vom Ober "geschaetzte" Restzeit enthaelt.
    */
    private Gruppe geschaetzt(Gruppe gr) {
        int echt; //tatsaechliche Restzeit
        double restzeit_ober, //geschaetzte Restzeit des Obers
               fehler; //Fehlerwert
        Gruppe temp; //Kopie der Gruppe
        
        //neues Objekt anlegen
        temp = new Gruppe();
        
        //Daten kopieren
        temp.setAnzahlGaeste(gr.getAnzahlGaeste());
        temp.setDauer(gr.getDauer());
        temp.setStartIndex(gr.getStartIndex());
        temp.setEndeIndex(gr.getEndeIndex());
        
        //tatsaechliche Restzeit auslesen
        echt = gr.getRestzeit();
        
        //maximale Abweichung bestimmen
        fehler = (echt / 100.0) * (100.0 - (double) erfahrung);
        
        //Restzeit "abschaetzen"
        restzeit_ober = (echt - fehler) + Math.random() * 2.0 * fehler;
        
        //mindestens eine Restzeit von 1
        if(restzeit_ober < 1.0)
            restzeit_ober = 1.0;
        
        //Restzeit setzen
        temp.setRestzeit((int) restzeit_ober);
        
        //Gruppe mit geschaetzter Restzeit zurueckgeben
        return temp;
    }
    
    /*
      Thread, der ankommende Gruppen simuliert und platziert.
    */
    class SimulationThread extends Thread {
        public void run() {
            Gruppe gruppe; //Gruppe, die platziert werden soll
            
            //Objekt der Klasse Gruppe anlegen
            gruppe = new Gruppe();
            
            //solange Stop-Button nicht gedrueckt wird
            while(button3.isEnabled()) {
                //alle Gruppen durchgehen
                for(int i = 0; i < gruppen.size(); i++) {
                    //naechster Tick
                    gruppen.get(i).nextTick();
                    
                    //pruefen, ob Gruppe gegangen ist
                    if(gruppen.get(i).getDauer() == 0) {
                        //Gruppe aus ArrayListe entfernen
                        gruppen.remove(i);
                        i = i - 1;
                    }
                }
                
                //ArrayListe (Gruppen des Obers) leeren
                gruppen_ober.clear();
                        
                //Ober schaetzt Restzeiten der Gruppen ab
                for(int i = 0; i < gruppen.size(); i++)
                    //hinzufuegen einer Gruppe mit abgeschaetzter Restzeit
                    gruppen_ober.add(geschaetzt(gruppen.get(i)));
                    
                //mehrere Gruppen pro Tick
                int h; //Hilfsvariable
                
                //Anzahl der Gruppen, die in diesem Tick kommen bestimmen
                if(max_gruppen_pro_tick == 1)
                    h = (int) (Math.random() + 0.5);
                else
                    h = (int) Math.ceil(Math.random() * max_gruppen_pro_tick);
                
                for(int i = 0; i < h; i++) {
                    //Gruppe mit Werten fuellen
                    gruppe.erstelleGruppe(max_gaeste_pro_gruppe, max_dauer);
                    
                    if(platziereGruppe(gruppe) == 0) {
                        //Gruppe konnte nicht platziert werden
                        
                        //Text der TextArea setzen
                        textarea.setText("Der Ober aergert sich.\n" +
                                         "Eine Gruppe von " +
                                         gruppe.getAnzahlGaeste() +
                                         " Gaesten\n" +
                                         "konnte nicht platziert werden.");
                        
                        //TextArea sichtbar machen
                        textarea.setVisible(true);
                        
                        try {
                            restaurant.repaint();
                            
                            //doppelt solange warten, wie ein Tick dauert
                            for(long j = 0; (j < wait / 100)
                                && (button3.isEnabled() == true); j++)
                                Thread.sleep(200);
                        } catch (InterruptedException e) { }
                        
                        //TextArea nicht sichtbar machen
                        textarea.setVisible(false);
                        
                        //nur eine Fehlermeldung pro Tick
                        break;
                    } else
                        //neues Objekt erzeugen
                        gruppe = new Gruppe();
                }
                
                //Zeichnen des Turmrestaurants
                restaurant.repaint();
                
                //Warten
                try {
                    for(long i = 0; (i < wait / 100)
                        && (button3.isEnabled() == true); i++)
                        Thread.sleep(100);
                } catch(InterruptedException e) { }
            }
            
            //ArrayListen leeren
            gruppen.clear();
            gruppen_ober.clear();
            freiePlaetze.clear();
            
            //leeres Turmrestaurant zeichnen
            restaurant.repaint();
        }
    }
    
    /*
      Thread, der die Gruppen der Schueler kommen laesst und platziert
    */
    class StreichThread extends Thread {
        public void run() {
            Gruppe gruppe;
            Streich streich;
            
            //Streich-Objekt anlegen, das die Schuelergruppen liefert
            streich = new Streich(n);
            
            //solange nicht der Stop-Button gedrueckt wird
            while(button3.isEnabled()) {
                //alle Gruppen durchgehen
                for(int i = 0; i < gruppen.size(); i++) {
                    int anzahl; //Hilfsvariable
                    
                    //Anzahl der Gaeste der Gruppe merken 
                    anzahl = gruppen.get(i).getAnzahlGaeste();
                    
                    //naechster Tick
                    gruppen.get(i).nextTick();
                    
                    //pruefen, ob Gruppe gegangen ist
                    if(gruppen.get(i).getDauer() == 0) {
                        //Schuelergruppe geht
                        streich.GruppeGeht(anzahl);
                        
                        //Gruppe aus ArrayListe entfernen
                        gruppen.remove(i);
                        i = i - 1;
                    }
                }
                
                //ArrayListe (Gruppen des Obers) leeren
                gruppen_ober.clear();
                        
                //Ober schaetzt Restzeiten der Gruppen ab
                for(int i = 0; i < gruppen.size(); i++)
                    //hinzufuegen einer Gruppe mit abgeschaetzter Restzeit
                    gruppen_ober.add(geschaetzt(gruppen.get(i)));
                
                //naechste Gruppe von Schuelern kommen lassen
                gruppe = streich.naechsteGruppe();
                
                //pruefen, ob eine Gruppe kommt
                if(gruppe != null) {
                    //Gruppe setzen
                    if(platziereGruppe(gruppe) == 0) {
                        //Gruppe konnte nicht platziert werden
                        
                        //Text der TextArea setzen
                        textarea.setText("Der Ober aergert sich.\n" +
                                         "Eine Gruppe von " +
                                         gruppe.getAnzahlGaeste() +
                                         " Gaesten\n" +
                                         "kann nicht platziert werden.\n" +
                                         "Benoetigte Schueler: " +
                                         streich.getBenoetigt());
                        
                        //TextArea sichtbar machen
                        textarea.setVisible(true);
                        
                        //Gruppe geht, weil sie nicht platziert werden kontne
                        streich.GruppeGeht(gruppe.getAnzahlGaeste());
                        
                        try {
                            restaurant.repaint();
                            
                            //doppelt solange warten, wie ein Tick dauert
                            for(long j = 0; (j < wait / 100)
                                && (button3.isEnabled() == true); j++)
                                Thread.sleep(200);
                        } catch (InterruptedException e) { }
                        
                        //TextArea nicht sichtbar machen
                        textarea.setVisible(false);
                    }
                }
                
                //Zeichnen des Turmrestaurants
                restaurant.repaint();
                
                //Warten
                try {
                    for(long i = 0; (i < wait / 100)
                        && (button3.isEnabled() == true); i++)
                        Thread.sleep(100);
                } catch(InterruptedException e) { }
            }
            
            //ArrayListen leeren
            gruppen.clear();
            gruppen_ober.clear();
            freiePlaetze.clear();
            
            //leeres Turmrestaurant zeichnen
            restaurant.repaint();
        }
    }
    
    /*
      Ein Panel, auf dem das Turmrestaurant gezeichnet werden kann.
    */
    class Zeichnen extends JPanel {
      private static final long serialVersionUID = 1L;
        
      //wird auch beim aendern der Groesse des Frames ausgefuehrt
      public void paintComponent(Graphics g) {   
        try {
          int x_mitte, //x-Koordinate des Mittelpunktes
              y_mitte, //y-Koordinate des Mittelpunktes
              h; //Hilfsvariable
                 
          double radius, //Radius des Turmrestaurants in Pixeln
                 radius_tisch, //Radius eines Tisches in Pixeln
                 winkel; //Mittelpunktswinkel zwischen den Tischen
                  
          //bestimmen der Mitte des Zeichenpanels
          x_mitte = restaurant.getWidth() / 2 - 4;
          y_mitte = restaurant.getHeight() / 2 - 14;
            
          //kleinere der beiden Frameseiten bestimmen
          h = (restaurant.getHeight() < restaurant.getWidth()) ?
              (restaurant.getHeight()) : (restaurant.getWidth());     
              
          //Radius des Turmrestaurants bestimmen
          radius = h / 2.0 - h / 8.0;
             
          //Radius eines Tisches bestimmen
          radius_tisch = (Math.PI * radius) / (n / 2.0);
                
          //Mittelpunktswinkel "zwischen"
          //zwei benachbarten Tischen bestimmen
          winkel = 360.0 / (double) n;
                
          //Farbe zum Zeichnen auf weiss setzen
          this.setBackground(Color.white);
                
          //loeschen des vorher Gezeichneten:
          //Farbe zum Zeichnen auf weiss setzen
          g.setColor(Color.WHITE);
          //Rechteck auf gesamtes Panel zeichnen
          g.fillRect(0, 0, restaurant.getWidth(), restaurant.getHeight());
                
          //Labels nicht sichtbar machen
          for(int i = 0; i < MAX_SITZPLAETZE; i++)
              rest.get(i).setVisible(false);
                
          //alle Sitzplaetze durchgehen
          for(int i = 0; i < n; i++) {
            //alle Gruppen durchgehen
            for(int j = 0; j < gruppen.size(); j++) {
              int start, //Startindex der Gruppe
                  ende, //Endindex der Gruppe
                  restzeit, //Restzeit der Gruppe
                  dauer; //Verweildauer der Gruppe
                     
              //Daten der Gruppe auslesen
              start = gruppen.get(j).getStartIndex();
              ende = gruppen.get(j).getEndeIndex();
              restzeit = gruppen.get(j).getRestzeit();
              dauer = gruppen.get(j).getDauer();
                         
              //pruefen, ob der Sitzplatz i zu einer Gruppe gehoert:
              if((start > ende && ((i >= start) ?
                 (i) : (i + n)) <= (ende + n) && (i + n) >= start)
              || (i <= ende && i >= start)) {
                int gruen; //Gruenanteil der Farbe
                            
                //Gruenanteil bestimmen (je nach Verweildauer der Gruppe)
                gruen = 90 + 
                       (int) ((165.0 / (double) max_dauer) * (double) dauer);
                          
                //Farbe zum Zeichnen der Gruppe festlegen
                g.setColor(new Color(240,
                                     345 - ((gruen > 255) ? (255) : (gruen)),
                                     0));
                           
                //Kreissektor zeichnen
                //(Mittelpunktswinkel ist abhaengig von Restzeit)
                //Koordinaten der Kreise werden berechnet mit:
                //x = x_m + r * cos(phi)
                //y = y_m + r * sin(phi)
                g.fillArc((int) (x_mitte +
                                 radius *
                                 Math.cos(Math.toRadians(winkel * i)) -
                                 radius_tisch / 2.0),
                          (int) (y_mitte +
                                 radius *
                                 Math.sin(Math.toRadians(winkel * i)) -
                                 radius_tisch / 2.0),
                          (int) radius_tisch,
                          (int) radius_tisch,
                          90,
                          (int) (((double) restzeit / (double) dauer) *
                                 360.0));
                           
                //Label zur Anzeige der Restzeit der Gruppe:
                     
                //Schriftgroesse anpassen (je nach Framegroesse)
                rest.get(i).setFont(new Font("Arial",
                                             Font.ITALIC | Font.BOLD, h / 40));
                          
                //Restzeit in Label schreiben
                rest.get(i).setText(String.valueOf(restzeit));
                         
                //Labelposition bestimmen
                rest.get(i).setLocation((int) (x_mitte +
                                        radius *
                                        Math.cos(Math.toRadians(winkel * i)) -
                                        rest.get(i).getWidth() / 2),
                                        (int) (y_mitte +
                                        radius *
                                        Math.sin(Math.toRadians(winkel * i)) -
                                        rest.get(i).getHeight() / 2));
                        
                //Label sichtbar machen
                rest.get(i).setVisible(true);
              }
            }
          }
                
          //Farbe zum Zeichnen auf grau setzen
          g.setColor(Color.GRAY);
                
          //Raender um die Sitzplaetze zeichnen
          for(int i = 0; i < n; i++) {
              g.drawArc((int) (x_mitte +
                               radius * Math.cos(Math.toRadians(winkel * i)) -
                               radius_tisch / 2.0),
                        (int) (y_mitte +
                               radius * Math.sin(Math.toRadians(winkel * i)) -
                               radius_tisch / 2.0),
                        (int) radius_tisch,
                        (int) radius_tisch,
                        0, 360);
          }
               
          //Textarea fuer die Mitteilung, wann sich der Ober aergert:
            
          //Schriftgroesse anpassen (je nach Framegroesse)
          textarea.setFont(new Font("Arial", Font.BOLD, h / 25));
                
          //TextArea-Position bestimmen
          textarea.setLocation(x_mitte - textarea.getWidth() / 2,
                               y_mitte - textarea.getHeight() / 2);
        } catch(IndexOutOfBoundsException e) {
            //Threadueberlagerung
            //Gruppen-Liste wurde waehrend dem Zeichnen geloescht
        }
      }
    }
}
