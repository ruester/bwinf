/*
  Klasse: Gruppe
          -> steht fuer eine Gruppe im Turmrestaurant
          -> beinhaltet Informationen:
             - die gesamte Verweildauer der Gruppe
             - wie lange die Gruppe noch bleibt
             - wo sie platziert ist
             - wie viele Gaeste in der Gruppe sind
             
  Methoden: - Getters/Setters
            - init
            - erstelleGruppe
            - nextTick
*/

class Gruppe {
    //Eigenschaften
    protected int anzahl_gaeste, //Anzahl der Gaeste der Gruppe
                  dauer,         //gesamte Verweildauer der Gruppe
                  restzeit,      //Restzeit der Gruppe
                  index_start,   //StartIndex der Gruppe
                  index_ende;    //EndIndex der Gruppe
    
    //Konstruktor
    public Gruppe() {
        init();
    }

    /*
      Methode initialisiert die Eigenschaften der Gruppe.
    */
    public void init() {
        this.anzahl_gaeste = 0;
        this.dauer = 0;
        this.restzeit = 0;
        this.index_start = -1;
        this.index_ende = -1;
    }
    
    /*
      Methode erstellt eine Dauer und die Anzahl der Gaeste der Gruppe.
      
      benoetigte Parameter:
        max_anzahl_gaeste - maximale Anzahl an Gaesten,
                            aus der die Gruppe bestehen darf
        max_dauer - maximale Verweildauer, die die Gruppe besitzen darf
    */
    public void erstelleGruppe(int max_anzahl_gaeste, int max_dauer) {
        //Anzahl der Gaeste bestimmen
        //durch ceil: mindestens 2 Gaeste
        this.anzahl_gaeste = (int) Math.ceil(Math.random() *
                                   (max_anzahl_gaeste - 1)) + 1;
        
        //Verweildauer der Gruppe bestimmen
        this.setDauer((int) Math.ceil(Math.random() * (max_dauer - 1)) + 1);
    }
    
    /*
      Methode vermindert die Restzeit der Gruppe um 1 und
      setzt die Werte der Gruppe zurueck, wenn die Restzeit auf 0 ist.
    */
    public void nextTick() {
        this.restzeit--;
        if(this.restzeit == 0)
            init();
    }
    
    //Getters/Setters:
    
    //Indizes
    public void setStartIndex(int index) {
        this.index_start = index;
    }
    
    public int getStartIndex() {
        return this.index_start;
    }
    
    public void setEndeIndex(int index) {
        this.index_ende = index;
    }
    
    public int getEndeIndex() {
        return this.index_ende;
    }
    
    //Restzeit
    public void setRestzeit(int r) {
        this.restzeit = r;
    }
    
    public int getRestzeit() {
        return this.restzeit;
    }
    
    //Anzahl Gaeste
    public void setAnzahlGaeste(int g) {
        this.anzahl_gaeste = g;
    }
    
    public int getAnzahlGaeste() {
        return this.anzahl_gaeste;
    }
    
    //Dauer
    public void setDauer(int d) {
        this.dauer = d;
        this.restzeit = d;
    }
    
    public int getDauer() {
        return this.dauer;
    }
}
