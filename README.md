# MatrixProject

Am luat conceptul clasic al jocului "Snake" si l-am adus intr-o alta forma:
In loc sa controlez un sarpe lung care creste pe masura ce mananca mere, am hotarat sa fac ceva diferit.

Jucatorul devine un simplu punct mobil:
Am hotarat sa-i ofer jucatorului controlul asupra unui punct mobil pe ecran. Nu mai este vorba despre un sarpe, ci despre un simplu punct care se deplaseaza in toate directiile. 

Misiunea: mancarea cat mai multor mere:
Misiunea jocului ramane aceeasi: Trebuie sa te concentrezi pentru a colecta cat mai multe mere care apar aleatoriu pe ecran. 

Controlarea timpului:
Si pentru a adauga o nota de competitie, am limitat timpul de joc la 15 secunde. In aceasta perioada scurta, trebuie sa te concentrezi la maxim si sa aduni cat mai multe mere posibil.

<strong>Cerinte pentru Meniul Jocului:</strong> 

Mesaj de Introducere la Pornirea Jocului:
Cand jocul este pornit, se afiseaza un mesaj de bun venit pe ecranul LCD pentru cateva secunde. 

Meniul cu Categoriile Urmatoare:
a) Start Joc: Aceasta optiune permite jucatorului sa inceapa jocul. <br></br>
b) Control Lumina LCD (obligatoriu): Jucatorul poate ajusta luminozitatea ecranului LCD. Aceasta ajustare trebuie sa afecteze un fir LED conectat direct la 5V. 
c) Control Lumina Matrice (folosind functia setIntensity din biblioteca ledControl): Jucatorul poate ajusta luminozitatea matricei de LED-uri. Se afiseaza toate ledurile de pe matrice pentru a indica nivelul de lumina selectat. 
D) Despre: Acesta optiune ofera detalii despre creatorul jocului.

In Timpul Jocului:
In timp ce jocul se desfasoara, se afiseaza toate informatiile relevante pentru jucator: timpul raman si scorul.
