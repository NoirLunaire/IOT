# Hardware

## Proc

## UART
Universal Asynchronous Receiver Transmitter
### Registres
	0x4	UARTDR
	0x18	UARTFR
		6 RXFF	activé si la FIFO de reception est pleine
		5 TXFF	activé si la FIFO de transmission est pleine
		4 RXFE	activé si la FIFO de reception est vide
	0x38	UART IMSC
		5 TXIM	active/désactive les interruptions de transmission
		4 RXIM	active/désactive les interruptions de réception
	0x44	UART ICR
		5 TXIC	clear l'interruption de transmission
		4 RXIC	clear l'interruption de réception

## VIC
Vector Interrupt Controller

### Registres
	0	IRQSTATUS
		les interruptions actuellement soulevés
	4	IFQSTATUS
		pour les interruptions de type FIQ
	0xC	INTSELECT
		sélectionne le type d'interruption
	0x10	INTENABLE
		active/désactive les interruptions
	0x14	INTENABLECLEAR
		clear les interruptions

# Software

### Kernel
Notre noyau est composé des sections habituel d'un ELF, défini dans kernel.ld
exception.s est le premier fichier chargé, il contient divers handler, notamment pour l'isr

### GDB / R2
`make debug`
Lance le serveur

`gdb -ex "target remote localhost:1234" build/kernel.elf`
Pour lancer gdb et se connecter au serveur
`r2 -d gdb://localhost:1234`
Pour lancer radare2 et se connecter au serveur

`b *addr` / `db addr`
ajouter un breakpoint

`disas` / `pdi`
affiche la fonction désassemblé

`continue` / `next` / `dn` / `dc`
continue ou avance à la prochaine instruction

### Ring
les lectures / écritures sur un ring n'ont pas de problème de synchronisation, on les préfères donc quand on doit gérer les lectures écritures avec des interruptions (car elles sont asynchrones)


