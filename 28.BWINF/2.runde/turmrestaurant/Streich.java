/*
  Klasse: Streich
          -> liefert die Gruppen fuer einen Streich,
             damit sich der Ober aergert
          -> beinhaltet Informationen:
             - wie viele Schueler fuer den Streich benoetigt werden
             - wie viele Gruppen schon in das Turmrestaurant geschickt wurden
             - wie viele Schueler nicht im Turmrestaurant sind 
             - Anzahl der Sitzplaetze im Restaurant
             
  Methoden: - Getter
            - naechsteGruppe
            - getBenoetigt
            - GruppeGeht
*/

class Streich {
    //Eigenschaften
    private int benoetigt, //Anzahl der Schueler,
                           //die fuer den Streich benoetigt werden
                count,     //Zaehlvariable fuer Anzahl der geschickten Gruppen
                schueler,  //Anzahl der Schueler, die nicht im Restaurant sind
                a,         //Hilfsvariable -> n / 4
                n;         //Anzahl Sitzplaetze im Restaurant
        
    //Konstruktor
    public Streich(int n) {
        this.count = 0;
        this.n = n;
        this.a = (int) Math.floor(((double) this.n) / 2.0) - 2;
        this.benoetigt = this.a + 6;
        this.schueler = this.benoetigt;
    }
    
    //Getter
    public int getBenoetigt() {
        return this.benoetigt;
    }
    
    /*
      Methode liefert die naechste Gruppe,
      die benoetigt wird, um den Ober zu aergern.
      
      Rueckgabewerte:
        null, wenn nicht genug Schueler da sind,
        sonst eine Gruppe, die den Ober aergert bzw. bald aergern wird
    */
    public Gruppe naechsteGruppe() {
        //pruefen, ob genug Schueler da sind
        if(this.schueler < 2)
            return null; //nicht genug Schueler da, um eine Gruppe zu bilden
        
        Gruppe temp; //Gruppe, die zurueckgegeben wird
        int dauer,   //Verweildauer der Gruppe
            anzahl;  //Anzahl der Schueler in der Gruppe
        
        //neues Objekt anlegen
        temp = new Gruppe();
        
        //wie viele Schuelergruppen wurden schon geschickt
        switch(this.count) {
            //Verweildauer und Anzahl der Schueler festlegen
            //(je nach Anzahl der geeschickten Gruppen)
            case 0:
                dauer = 360;
                anzahl = 2;
            break;
            
            case 1:
                dauer = 40;
                anzahl = this.a;
            break;
            
            case 2:
                dauer = 10;
                anzahl = 2;
            break;
            
            case 3:
                dauer = 360;
                anzahl = 2;
            break;
            
            default: //aergern
                dauer = 360;
                anzahl = this.a + 2;
        }
        
        //pruefen, ob genug Schueler da sind
        if(this.schueler < anzahl)
            return null; //nicht genug Schueler da
        
        //Verweildauer und Anzahl der Schueler setzen
        temp.setDauer(dauer);
        temp.setAnzahlGaeste(anzahl);
        
        //Schueleranzahl, die "draussen" stehen verringern
        this.schueler -= temp.getAnzahlGaeste();
        
        //Anzahl der geschickten Gruppen erhoehen
        this.count++;
        
        //Gruppe zurueckgeben
        return temp;
    }
    
    /*
      Methode wird aufgerufen,
      wenn eine Schuelergruppe aus dem Turmrestaurant geht
    */
    public void GruppeGeht(int anzahl) {
        //Anzahl der Schueler, die "draussen" stehen wird erhoeht
        this.schueler += anzahl;
    }
}
