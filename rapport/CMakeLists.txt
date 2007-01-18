SET(INPUT_FILES chapitres/introduction.tex 
	             chapitres/graham.tex 
			       chapitres/johnson.tex 
			       chapitres/smith.tex 
			       chapitres/tamandua.tex
			       )
SET(LATEX_OUTPUT_PATH .)
ADD_LATEX_DOCUMENT(rapport.tex
	INPUTS ${INPUT_FILES}
	IMAGE_DIRS .
	DEFAULT_PDF)
