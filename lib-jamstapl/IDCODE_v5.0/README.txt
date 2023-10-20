README File for IDCODE.JAM file (IDCODE.ZIP)
Version 5.0  2007/12/24 ELL

INTRODUCTION
============
The IDCODE.JAM file is an ASCII text file in Jam STAPL language syntax that 
will check the continuity of the IEEE 1149.1 chain and will read the IDCODE
value from each device in a IEEE 1149.1 chain.  It does not require the 
user to specify any characteristics of the devices in the chain.

PURPOSE
=======
This file is used to interrogate a IEEE 1149.1 chain of devices using the
Jam STAPL language syntax.  Such an interrogation is usually used to help 
debugging ISP issues when porting the Jam STAPL ByteCode Player (or Jam 
STAPL Player) to various embedded processors.  The file provides a known 
good starting point, from which to identify potential hardware or software 
problems.

USAGE
=====
To use this file, run a Jam STAPL Player on this file and specify the action
called READ_IDCODE.

For Altera's Jam STAPL Player (2.X versions), the following command-line
will execute the file:
  jam -aread_idcode idcode.jam

Alternatively, for Altera's Jam STAPL ByteCode Player, the following two
steps are
 (1) Compile it:  jbc idcode.jam idcode.jbc
 (2) Execute it:  jbi -aread_idcode idcode.jbc

CONTENTS
========
The three files in this Zip file are:
  IDCODE.JAM      Jam STAPL file for Checking the IEEE 1149.1 chain
  IDCODE.JBC      Its compiled equivalent for Jam STAPL Bytecode Players
  README.TXT      This readme file

ADDITIONAL INFORMATION
======================
For a copy of the Jam STAPL specification (JEDEC specification JESD-71), go 
to the JEDEC website:  http://www.jedec.org/download/jedec/jesd71.pdf

For more information about usage of the Jam STAPL standard, including a copy
of the Jam STAPL Byte-Code compiler and additional documentation go to: 
http://www.altera.com/jam.

OLDER VERSIONS
==============
Previous to this version of the IDCODE.JAM file, there were a couple previous
versions.  These older versions are also available as IDCODE_OLD.ZIP.  This
IDCODE_OLD.ZIP file contains an implementation that reads a single device's
IDCODE but requires the user to provide chain topology information.  This
older version is provided in Jam STAPL (standardized version) and in Jam 1.1
(pre-standardized) versions.

Version 2.01 added additional non-Altera devices to the list.
Version 2.02 added couple additional devices and changed action name to
  read_idcode
version 3.0 added Stratix, Stratix GX, Cyclone, ApexII, APEX20KC and EPC8
  devices support.
version 4.0 added MAX II, Stratix II and Cyclone II devices support.
version 5.0 added MAX IIZ, Stratix III, Cyclone III and Arria GX devices support.


QUESTIONS OR PROBLEMS
=====================
Contact Altera Applications at 1-800-800-3753 or e-mail support@altera.com. 
For 24-hr support check the Atlas Solutions database on the Altera Home Page. 
(http://www.altera.com)

