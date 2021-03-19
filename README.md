# YACC1-2020 (Yet Another Custom CPU)

Finally time to try building a custom CPU from basic TTL and other "simple" ICs (no 74ls181 ALU, that would be cheating :-) ). 

The YACC1-2020, provides the tools and resources to learn computer science starting from the basics of digital logic through to a fully working custom CPU with a user defined instruction set, assembler, BASIC interpreter, and one day an operating system with filesystem, command line interpreter, and utilities.

The project is currently under development as a personal project. The basic idea has been kicking around inside my head for years (47 years to be exact), but until recently never made it to the top of my list (I did make an attempt a few years back but life got in the way). Now I am exploring if this project might be interesting to others, perhaps as an educational tool or maybe just a fun project.

Here is a [video of the machine in operation](https://youtu.be/6pjIE4_MxIA) and some photos can be found in the Photos & Videos subdirectory in this repository

The basic system is comprised of xx cards:
- Memory card with 64k RAM and 32K ROM (total address space 64K)
	Memory card also contains two temporary registers
- Register card, each with 4 16 bit registers that can drive data or address bus, although the system can operate with one register card anything really useful will require two cards


## Motivation

Like many others, back in the mid 1970s I had the opportunity to learn both computer programming and microcomputer hardware simultaneously. For me it all started with programming BASIC on a timeshare HP machine via a terminal and/or optical hollerith cards. My math teacher who also taught computer programming allowed us to use the termoianl if we finished our work early. At home I also dabbled with digital circuits and when Popular Electronics published the COSMAC ELF project I was hooked. Now I wonder if for some students when learning computer science does the hardware seem too much like a magic box.

If this project is of intertest, perhaps your one of the following:<br/>
- High school computer science educators<br/>
- Freshman and sophomore college computer science educators<br/>
- Electronics or Retro computer enthusiast<br/>
- Or just generally interested
	
Please reach out to ken at cottageworker dot com


## Directories

### Data Sheets:
		Useful data sheets, manuals, references.
			(These are all readily available online but if I inadvertently violated copyright
			please let me know and I will remove immediately)

### General Notes:
		System wide notes, more specific notes in various subdirectories
		
### PCB:
		PCB designs, there are two different subdirectories:
			Production: These are the latest fully functioning designs, most likely currently in use
			Working - Underdevelopment: These are the newest versions of the boards in design
		Each board has or will have a "theory of operation" document
			
### Photos & Videos:
		Some sharable files

### Presentations:
		Presentations related to the YACC1-2020 project
		
### Software:
		All software related to the project, see subdirectory for more information
		
### Utilities:
		Arduino: Configuration files for Arduino code
		Clocker: External Arduino based to tool for single step debug operations
		Eagle: Configuration files for Eagle
		Waveforms: Timing diagrams for cpu instructions, probably should move to software area

### YACC1 Connector - V3.2.pdf
		Current bus connector specifications
		
<br/><br/> 
-- Thanks for taking a look --

