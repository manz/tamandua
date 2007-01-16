rapport.pdf : $(wildcard *.tex) $(wildcard chapitres/*.tex) rapport.toc
	pdflatex rapport.tex

rapport.toc :
	pdflatex rapport.tex

clean :
	rm -f *.aux *.log

deepclean : clean
	rm -f *.toc
