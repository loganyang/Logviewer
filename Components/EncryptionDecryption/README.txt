R.Wobst, @(#) Aug 31 2011, 10:46:07

ImportExport - Stand
====================

STRUKTUR IN SVN:

  Die Software befindet sich in SVN unter

  svn_trunk/Platform/Master/Components/ImportExport

  und besteht derzeit aus den Subdirectories

  AbstractFile		# vgl. Kommentar zu ReadArchive unten
  AES			# AES-Verschl�sselung und -Entschl�sselung
  CompressEncrypt	# Komprimieren, ggf. Chiffrieren, Split in Chunks
  CryptoService		# Key handling, HMACs+Hash berechnen, Chiffrierung
  DecryptUncompress	# Umkehrung zu CompressEncrypt
  General		# allg. Hilfsklassen und Konstanten
  HMAC			# HMAC-Berechnung (nicht in Qt enthalten)
  ReadArchive		# Einlesen, Checken und Auspacken eines Archivs
  WriteArchive		# Erzeugen eines Archivs
  Build			# Erzeugen einer Bibliothek libImportExport.a

  Jedes Directory enth�lt die Subdirectories Include, Source und Test.
  In jedem Test-Directory befindet sich ein Projektfile mit Namen
  utTest*.pro mit Unittests f�r diese Einheit. Es wird vorerst nur im
  Debugmode gebildet. Vom Buildbot aus werden nur die komplexen
  Unittests in ReadArchive und WriteArchive ausgef�hrt; Fehler in
  Submodulen wirken sich sofort auf diese beiden Testgruppen aus.

  Zus�tzlich finden sich im Subdirectory Build noch die �blichen
  *pro/*pri-Files, um Bibliotheken zu erzeugen. Diese Bibliotheken
  werden von Unittests in ReadArchive und WriteArchive genutzt.

  Die *.pri-Files in ReadArchive/Test und WriteArchive/Test sind
  modifizierte Kopien der pri-Files von
  svn_trunk/Platform/Master/Test/Platform.pri, weil die Variable
  COMPONENTDIR bei der ImportExport-Komponente auf eine Ebene
  h�her verweisen muss.

  Au�erdem gibt es noch in ReadArchive, WriteArchive und
  DecryptUncompress Directorys Test/testresults mit Bin�rfiles f�r 
  Unittests. In ReadArchive/Test/testresults findet sich ein
  Unterprojekt GenerateTestArchives.pro f�r die Erzeugung von
  Testarchiven.

  Eine Reihe von Pythonskripten dient der Erzeugung weiterer Testdaten
  bzw. der Analyse erzeugter Archive und ist im Quellcode dokumentiert.


API
===

Als API dienen momentan nur die beiden Funktionsrufe

void WriteArchive(QByteArray archive_name,
	   QList<QByteArray> filelst,
	   bool encrypt = false,
	   qint16 key_version=0);

f�r den Export und

void ReadArchive(QByteArray archive_name,
	   AbstractFile* fd,
	   QByteArray purpose,
           QByteArray keydata = QByteArray());

f�r Import/Viewer/Checker. Details finden sich in der Doxygen-Dokumentation.

Bei Fehlern wird die Ausnahme ImexException geworfen und muss vom Caller
angefangen werden. �ber deren Methode getMsg() kann die Fehlernachricht
ermittelt werden (vgl. Doxygen-Dokumentation).

Der Parameter fd bei ReadArchive ist eine abstrakte Fileschnittstelle,
die beim Import das Schreiben auf physische Files realisiert (mit den
Methoden open(), write() und close()). Beim Viewer wird statt dessen ein
QHash erzeugt mit Filenamen als Keys und Fileinhalten als Values. Beim
Checker werden nur HMACs �berpr�ft und die Filenamen in einem QSet
gesammelt.

Beide Funktionen sind in der Bibliothek libImportExport.a implementiert.


ARCHIVFORMAT
============

Das Archivformat ist im SDS-Manuskript genau beschrieben und kritisch.
Es sollte nicht ver�ndert werden, obwohl ein Feld "Formatversion" im
Header f�r alle F�lle enthalten ist.

Auf Grundlage dieser Informationen ist es nicht schwer, ein Archiv auch
mit anderen Programmen auszupacken, vorausgesetzt, man kennt die
Schl�ssel.

Beim Entpacken wird jeweils nur ein Schl�ssel ben�tigt: Beim Import ein
Schl�ssel C, der aus dem "Generalschl�ssel" B und einem festen String S0
berechnet wird und absichert, dass nur von gleichen Stainern (bzw. von
Leica) generierte Archive importiert werden k�nnen. Der
"Generalschl�ssel" B wird vom Viewer zum �berpr�fen der Integrit�t
genutzt, wobei Ger�tenummer und Folgenummer bekannt sein m�ssen (die aus
dem Header extrahiert werden k�nnen). Den "Geheimschl�ssel" A
schlie�lich darf nur Leica kennen; mit ihm lassen sich Archive
unabh�ngig von evtl. kompromittierter Viewer-Software pr�fen.

Das Format und die Art der HMAC-Berechnung sichern Integrit�t,
Reihenfolge und Vollst�ndigkeit von Header und Archiveintr�gen ab.


ART DER NUTZUNG
===============

Export:

F�r den Export soll WriteArchive in einer gesonderten Applikation
gestartet werden, mit niedriger CPU-Priorit�t (nice-Kommando) und
I/O-Priorit�t (ionice-Kommando). Archivname (und damit auch die
Seriennummer), Fileliste und Encryption Flag werden dabei als
Kommandozeilenargumente �bergeben. Die Fehlermeldungen und ggf.
Warnungen k�nnen dabei �ber eine Pipeline vom Caller empfangen werden,
wenn der Prozess mit QProcess gestartet wird.

Sinn des Starts einer gesonderten Applikation ist es, zeitkritisch
Prozesse nicht zu behindern. Das ist beim Export �ber Remote Care
wichtig.

Entweder sind alle Archiveintr�ge chiffriert (Logfiles), oder alle im
Klartext (genauer, nur komprimiert). F�r den Viewer und Remote Care
werden also i.a. zwei Archive erzeugt (eines davon chiffriert).

Es kann sinnvoll sein, beim Export aus irgendeinem Grund nicht lesbare
oder nicht existente Files mit einer Warnung zu �berspringen, damit z.B.
Remote Care oder Viewer noch Informationen zum Auswerten bekommen.

Ebenso k�nnte es noch einen zus�tzlichen Parameter "backup" geben: Das
Archiv wird nur zum Backup erzeugt. Der Index in der Hash Chain braucht
dabei nicht erh�ht zu werden.

Der Import kann ausschlie�lich vom Nutzer angesto�en werden uns sollte
wegen der entsprechenden �berpr�fungen der Inhalte innerhalb der
Mastersoftware laufen.

Denkbar w�re eine Importoption (Parameter 'purpose') "Dummy", die HMACs
nicht pr�ft. Allerdings braucht man das Keymanagement zum Auswerten
chiffrierter Archive unbedingt.

Alles Logging muss vom Caller aus geschehen.


TODO
====

- Die Namen der beiden Files, in denen die Schl�ssel mit Counter (auf
  NAND-Flash) und der Counter auf der SD-Karte gespeichert werden, sind
  vorerst noch in General::Constants fest eingebrannt. Sie sollten als
  Parameter �bergeben werden. Nach jetzigem Stand muss das �ber
  ReadArchive und WriteArchive geschehen, die es an CryptoService
  weitergeben.

  Sollte das Ger�t mit Festplatte statt SD-Karte gebaut werden, entf�llt
  die gesamte Logik f�r den auf SD-Karte gespeicherten
  Hashchain-Counter. Der Konstruktor von CryptoService (ab Zeile 105)
  sollte dann stark vereinfacht werden.

- Das Caller Interface existiert noch nicht, das Read/WriteArchive ruft
  und Exceptions auswertet (und beim Export die Error messages an den
  Caller sendet).

- Der Caller bei der Exportfunktion sollte die Liste der Filenamen
  aus einem XML-Archiv entnehmen. Shell-Metazeichen k�nnten hier
  sinnvoll sein, um z.B. Gruppen von Logfiles mit dynamisch generierten
  Namen einzupacken (QRegExp::Wildcard mit Shell-Option,
  QDir::listEntry() mit Filter).

- Die Archivnamen f�r Backup-Archiv und Logfile-Archiv m�ssen noch
  festgelegt werden, genauer, der 'info' Teil in

  Colorado_info_serialnumber_timestamp

  (vgl. SRSImportExport.doc 3.9.5 und SDSImportExport.doc 4.1.1).
