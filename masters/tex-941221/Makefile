LATEX = latex2e

all: thesis.dvi thesis.ps
	touch remake

thesis.ps: thesis.dvi
	dvips -o $@ $^

thesis.dvi: thesis.tex intro.tex trace.tex append.tex results.tex simulation.tex remake
	$(LATEX) thesis.tex

remake:
	touch remake

clean:
	rm *.aux *.log *.dvi *~
