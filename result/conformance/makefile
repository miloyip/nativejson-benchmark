%.html: %.csv template.php
	php template.php $< > $@

CSVFILES = $(basename $(wildcard *.csv))
all: $(addsuffix .html, $(CSVFILES))
