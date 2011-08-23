# 29. Bundeswettbewerb Informatik - 2. Runde
# Aufgabe 3 - Traumdreiecke
#
# Autor: Matthias Rüster
#
# E-Mail: ruester@molgen.mpg.de

FITDAT = $(addsuffix .fitness,$(basename $(wildcard *.td)))

TDDAT = $(wildcard *.td)

# Anzahl Ebenen
n = -1

# Populationsgroesse
p = -1

.PHONY: clean naechste-generation init

naechste-generation: 1.sel
	$(MAKE) -f generation.mk -B $(TDDAT)
	@rm -f $(wildcard *.sel)

%.td:
	./rekombination $@

1.sel: $(FITDAT)
	./selektion $(p)
	@rm -f $(wildcard *.fitness)

%.fitness: %.td
	./mutundfit $^

init: clean
	./initialisieren $(n) $(p)
	@rm -f td.loesung

clean:
	@rm -f $(wildcard *.sel)
	@rm -f $(wildcard *.td)
	@rm -f $(wildcard *.fitness)
