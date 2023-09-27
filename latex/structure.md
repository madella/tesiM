# TESI  
TITOLO: Interoperabilità
Analisi e sviluppo di middleware DDS per la gestione dei consumi in sistemi HPC

abstract
- Introduzione (contesto: power management, hpc, dds)
  - problematica 
  - contributi ( lista della spesa )
  non eccessivamente lunga
- Power Management in HPC enviroment
  - Stato dell'arte ( bottom up )
    -  Interfacce SO power manager (cpu-freq,rapl) [in-band]
    - Board Management Controller (OpenComputeProject) [out-of-band]
      - HDEEM Estende bmc con FPGA che legge le potenze HFrequency
    - Interfacce Alto lvello
      - ( vari attori )  
- DDS-RTPS & ROS
- REGALE (scheletro BSC)
- Lavoro svolto (REGALE library)
  - Caratterizzazione DDS HPC
  - Modello per utilizzo di DDS per REGALE
  - Sviluppo dei scheletro di due componenti
    - schema componenti
    - monitor 





STATO DELL?ARTE
- Sensori e attuatori per fare pm
  - Questi sensori e attuatori sono possibi
    - OUT of band
      - BMC 
    - in band
      - Power governors  
- 

# Meccanismi per la gestione dei consumi, servizi di power management, algoritmi di power management.

- Power management: gestione della Potenza. Qui l’obbiettivo è controllare la potenza assorbita. Indirettamente legata al raffreddamento (qui di parla di TDP - thermal design power – si fa l’assunzione di essere a regime, quindi a transitori termici esauriti), ma anche legata alla massima corrente erogabile da un alimentatore o dai pad del chip. In questo caso si parla di TDC, Thermal design current ma anche di peak current.

Quando si parla di TDP o TDC di norma quello che si vuole ottenere è fare power capping ovvero limitare la max power ad un certo valore. Se si parla invece di power management si può in modo erraneo anche includere energy management.

- Thermal management: gestione temperatura dinamica o statica. Ovvio.

- Energy management: gestione dell’energia e quindi anche di quello che si paga, e sostenibilità. Qui bisogna stare attenti in quanto l’estremo dell’energy management e riduzione del costo energetico è quello di spegnere tutto e andare al mare. Ovvero se non si fa compute o se si va molto piano si paga una penalità in performance molto elevata. Essendo un problema di trade off tra performance e riduzione dei consumi spesso si deve darsi una definizione di quella che è una performance accettabile o una riduzione di performance accettabile dentro la quale si cerca di ridurre l’energia. Fissata la riduzione di performance, minimizzare l’energia coincide con minimizzare la potenza. Da qui la confusione tra energy management e power management.

E’ doveroso differenziare tra energia e potenza in quanto ridurre la potenza non vuol dire per forza ridurre l’energia, e viceversa. Sono infatti famose due politiche, run fast and stop e go slow and low power. Ma qui ci arrivo dopo.

A volte si usa power management per includere tutti i tre, thermal, power e energy.

Finora abbiamo parlato di obbiettivi di power management. Passiamo al livello più basso.

## Meccanismi e componenti HW per il power management, ma anche thermal ed energy.

DFS: Dynamic Frequency scaling. Questo vuol dire che da qualche parte nell’archietettura c’è un registro che permette di cambiare la frequenza del PLL o FLL che eroga il clock alla CPU. Riducendo la frequenza di clock si ha un effetto lineare sulla riduzione della potenza dinamica.
Clock gating: da qualche parte nell’albero di clock c’è l’equivalente di un AND che può essere comandato tramite registro (throttling) o in modo automatico per silenziare il segnale di clock. Questo azzera la potenza dinamica.
Throttling – anche T state in terminologia ACPI. Questo vuol dire che è possibile salare dei cicli di clock in modo percentuale. Equivalente a DFS ma non proprio.
DVFS: Dynamic voltage e frequency scaling. In questo caso si varia anche il voltaggio insieme alla frequenza. Facendo ciò si riduce fino al cubo la potenza dinamica, si riduce in modo esponenziale il leakage. Aumenta il costo di design in quanto deve essere fatto il sign-off del core a diverse coppie di frequenza e tensione.  E’ necessario poter variare la tensione per delle zone del chip.  Servono o voltage regulator che permettono di variare la tensione, oppure degli LDO che sfruttano transistor per controllare un drop di tensione. A seconda dell’implementazione può essere il core a settare la nuova tensione e frequenza oppure un controllore di potenza. Vedi i P-state.
Power gating, o anche C-state. In questo caso viene rimossa l’aliomentazione ai core. A seconda dell’implementazione serve salvare il contesto o meno.
Power capping (RAPL) in questo caso esiste un registro che permette di fissare un consumo massimo per alcune sotto parti del chip. In realtà è realizzato da un controllore di potenza che tramite firmware controlla le manopole di sopra.
PVT sensors, process, voltage e temperature sensor. Servono per misurare la velocità del silicio, la tensione e la temperatura in zone del silicio.
 

# Algoritmi:

Come sfrutti le manopole di sopra per realizzare i goal di ancora più sopra.
Esempi sono Countdown, EAR, linux governors, ecc ecc.
 
Una parte fondamentale di tutti gli algoritmi di energy management è capire preventivamente di quanto si rallenterà l’applicazione riducendo le performance (frequency scaling o throttlingo o power capping RAPL). Questo perché non sempre è conveniente andare piano.
