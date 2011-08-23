#COMPILER= /usr/local/package/bin/javac
#LAUNCHER= /usr/local/package/bin/java

COMPILER= javac
LAUNCHER= java

all: clean TurmrestaurantMain

run: TurmrestaurantMain
	$(LAUNCHER) $^

TurmrestaurantMain: TurmrestaurantMain.class

%.class: %.java
	$(COMPILER) -g -Xlint $^

clean:
	rm -f *.class
