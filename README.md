**Nume:** Manolache Maria-Catalina
**Grupă:** 313CAb

## Segregated Free Lists

* Pentru a doua structura de date, ce va retine zonele de memorie ocupata, am
ales o lista dublu inlantuita in care nodurile vor avea ordinea crescatoare a
adreselor. 

### Functii elementare:
Pentru inceput, voi prezenta 6 functii care vor fi reutilizate pe parcurs:
* dll_add_nth_node si dll_remove_nth_node adauga si, respectiv, sterg un nod de
pe o anumita pozitie dintr-o lista oarecare.
* add_ascending cauta intr-o lista oarecare ultima adresa mai mica decat cea
data ca parametru, returnand pozitia urmatoare acesteia, pentru a fi folosita
alaturi de functiile de mai sus.
* find_node cauta intr-o lista oarecare nodul ce contine o anumita adresa,
returnand pozitia acestuia. Daca lista e goala, sau nodul nu s-a gasit, se va
returna -1.
* dll_create creeaza si initializeaza o lista noua.
* free_list elibereaza toate nodurile unei liste oarecare, toate campurile lor
si lista in sine.
* fill_lists va initializa nodurile din sfl cu adresele corespunzatoare, 
pornind de la adresa de inceput din structura sfl_t astfel: prima lista contine
noduri de 8 bytes, a doua de 16 s.a.m.d (adresele vor fi continue si diferenta
dintre ele va fi tot de 8, 16...) pana cand vom acoperi nr. de liste din sfl_t.

### Rezolvare:
* init_heap va creea si initializa vectorul de liste, apeland si fill_lists
ce se va ocupa de nodurile acestora. La final, voi calcula memoria totala 
pentru a o afisa in dump_memory.
* my_malloc calculeaza initial maximul disponibil pentru a fi alocat,
gestionand cazul "Out of memory". Voi parcurge vectorul de liste pentru a gasi
o lista cu noduri cu memoria mai mare sau egala decat cea necesara. 
    Pe cazul de egalitate, scot primul nod din lista, care va contine
adresa minima deoarece voi mentine ordinea crescatoare a adreselor, apoi gasesc
pozitia pe care il voi insera in lista de memorie ocupata.
    Daca memoria disponibila este mai mare decat cea necesara, voi fragmenta
nodul extras asemanator, calculand noua dimensiunea si adresa. Caut daca exista
deja o lista cu dimensiunea nodului liber obtinut prin fragmentare, daca nu, o
creez, deplasand celelalte liste in vector si realocandu-l pentru a insera
aceasta noua lista. Daca exista, voi insera nodul crescator dupa adresa in ea.
In final, voi adauga nodul in acelasi mod in lista de memorie ocupata.
    In ambele cazuri, cresc contoarele folosite la dump_memory.
* my_free cauta initial pozitia nodului cu adresa precizata si il scoate din 
lista de memorie alocata. Ca la my_malloc, se cauta o lista cu dimensiunea 
acestui nod, adaugandu-l in aceeasi maniera in ea. Daca nu se gaseste, se 
creeaza una noua, iar la final se elibereaza memoria nodului si se cresc 
contoarele pentru dump_memory.
* write, pentru inceput, parseaza linia de inceput si cauta nodul cu adresa 
data, returnand seg fault daca nu este gasit in lista de memorie alocata. 
    Daca toti bytes de scris incap intr-un singur nod, se verifica daca nodul 
avea deja informatii in el, pentru a le suprascrie partial sau total (daca
lungimea noilor date este mai mare). Se realoca memorie pentru campul data al
nodului, se copiaza informatiile si se adauga \0 pentru a crea un string valid.
    In caz contrar, se parcurg noduri in continuare pana s-au citit cati bytes
trebuiau sau pana se ajunge la finalul listei. Se verifica daca adresele lor 
nu sunt continue, caz in care se da seg fault. Se prelucreaza datele ca la 
cazul anterior.
* read este implementat asemanator cu write: se cauta nodul si se verifica 
daca datele de citit fac parte dintr-un singur nod. In caz contrar, se citesc
bytes pana la finalul listei sau pana s-au epuizat.
* dump_memory printeaza informatii despre heap continute in variabilele date
ca parametri sau in campurile din structuri, dupa formatul specificat.
* destroy_heap elibereaza memoria vectorului de liste si a listei de memorie
alocata.
### main: se apeleaza toate functiile de mai sus
