Χρησιμοποίησα τον αλγόριθμο forward-checking μαζί με conflict-directed-backjumping
από https://cse.unl.edu/~choueiry/Documents/Hybrid-Prosser.pdf , σελίδες 17-19 και 22-23
Επιλέγω την επομένη μεταβλητή για να συμπληρώσω στο σταυρόλεξο με το heuristic MRV και κάνω forward checking σε κάθε άλλη μεταβλητή που δεν έχω συμπληρώσει ακόμα η όποια επηρεάζεται από την τιμή της μεταβλητής.
Έχω επίσης ταξινομήσει το domain της κάθε μεταβλητής, με τις λέξεις που αφήνουν τις περισσότερες επιλογές στις επόμενες λέξεις έτσι ώστε να είναι στην αρχή του domain με αποτέλεσμά να επιλέγω πρώτα αυτές.
Όταν φτάνω σε αδιέξοδο και δεν υπάρχουν άλλες λέξεις να βάλω επειδή μηδενίζουν το domain κάποιον από τις επόμενες μεταβλητές,
γυρνάω στην πιο πρόσφατη μεταβλητή που τοποθέτησα που είτε επηρεάζει την μεταβλητή που βρίσκομαι τώρα, είτε επηρεάζει τις μεταβλητές που μηδένισε η μεταβλητή που βρίσκομαι τώρα.
Μεταγλώττηση : Το πρόγραμμα μεταγλωττίζεται με την εντολή "make crswrd" και το εκτελέσιμο που παράγεται είναι το "solve".
