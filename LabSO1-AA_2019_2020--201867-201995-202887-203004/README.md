LabSO1-AA_2019_2020--201867-201995-202887-203004
#Progetto per il laboratorio di sistemi operativi - AA 2019/2020

Paolo Barzon    - paolo.barzon@studenti.unitn.it    - 203004
Marco Bertolino - marco.bertolino@studenti.unitn.it - 202887
Cinzia Tonini   - cinzia.tonini@studenti.unitn.it   - 201995
Thomas Rigoni   - thomas.rigoni@studenti.unitn.it   - 201867

##Progetto II - Statistica frequenze caratteri

Abbiamo implementato la versione completa dei programmi richiesti per Ubuntu Bionic che comprende main, analyzer e report.
I file sorgente si trovano in src/, i file oggetto int bin/obj/, i file eseguibili in bin/.

Tramite make è possibile compilare il progetto attraverso il comando `make` o `make build`, eseguire una pulizia con `make clean` e visualizzare questo README tramit   e `make help`.

###Scelte implementative
- Il programma si appoggia a 2 pipe con nome presenti nella cartella /tmp/ di sistema per scambiare informazioni tra main e report e tra analyzer e report.
- Abbiamo deciso di implementare un garbage collector per assicuraci che al termine di un processo tutte le risorse allocate vengano liberate.
- Comandi bash utilizzati:
  - `find` per trovare i file contenuti all'interno di una cartella;
  - `wc` per trovare la dimensione in byte di un file;
  - `tput` per avere la largezza attuale del terminale;
- Inizialmente abbiamo usato lseek inserendo il cursore alla fine del file per avere la sua dimensione ma ci siamo accorti che con particolari file `.png` non funzionava correttamente.
- Per eseguire comandi bash abbiamo usato la funzione popen che opera su `FILE*`. Questo perchè non abbiamo trovato un modo migliore che non richiedesse di scrivere su File System.
- Se da main si richiede di analizzare un file una seconda volta questo viene automaticamente rianalizzato, ma se si richiede a report di visualizzarlo più volte vengono mostrati i dati più aggiornati senza una nuova analisi.

###Limiti e problemi esistenti
- N ed M non possono essere cambiati durante l'esecuzione di analyzer ma tramite main possono essere impostati a piacere tra le singole esecuzioni di analyzer.
- Il nome di un file da analizzare non può contenere le doppie virgolette `"`, ma è possibile lavorare con path che contengono spazi inserendoli in queste ultime.
- Il GC viene ripulito al termine del processo, alcune allocazioni di grande dimensione o evidentemente non utili in futuro vengono manualmente rimosse per risparmiare memoria.
- Bug: Analyzer con quantità grandi di file (circa maggiori di 120) a volte si blocca e non risponde.
- Bug: Raramente con grandi quantità di file e n, m elevati nell'uscire il programma genera un double free or corruption.