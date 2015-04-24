.PHONY: all clean

CSVFILES = $(basename $(wildcard *.csv))
all: $(addsuffix .html, $(CSVFILES))

performance_%.html: performance_%.csv performance.php
	php performance.php $< > $@

conformance.html: conformance.csv conformance.php
	php conformance.php $< > $@

clean:
	rm *.html