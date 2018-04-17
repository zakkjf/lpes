***********************************************************************************
**                                 AnetTest                                      **
**             Automated tests of network devices and programs                   **
**                                                                               **
**  Author: A. V. Titov                                                          **
**  Email: anettesttool@gmail.com                                                **
**  Site: http://anettest.sourceforge.net/													**
**                                                                               **
**  This program is free software; you can redistribute it and/or modify         **
**  it under the terms of the GNU General Public License as published by         **
**  the Free Software Foundation; either version 2 of the License, or            **
**  any later version.                                                           **
                                                                                 **
**  This program is distributed in the hope that it will be useful,              **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of               **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                **
**  GNU General Public License for more details.                                 **
                                                                                 **
**  You should have received a copy of the GNU General Public License            **
**  along with this program; if not, write to the Free Software                  **
**  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301    **
**  USA                                                                          **
**                                                                               **
***********************************************************************************

PURPOSES:

   1. Generating and tracing packets on channel level for Ethernet (sample generate_sequence_packets).

   2. Work with TCP sessions. Sending and receiving particular data over TCP connection. Imitating client or server (sample mail_reader).

   3. Sending defined data alternated with receiving defined data (sample ask_mac).

   4. Using own headers for any network protocol and packet's template (see headers).

   5. Using flexible language with conditions, variables, cycles and others.

   5. Creating own sniffers with concrete output. Powerful analysis of packets (sample http_parser).

   6. Work with statistic. Displaying general reports.

   7. Imitating network conversation using info from trace file (sample convtest1, convtest2).

   8. Modifying trace files (sample work_with_traces).
	
SITE WITH FULL DOCUMENTATION 
	
	http://anettest.sourceforge.net/
	http://anettest.sourceforge.net/anettest_doc_eng.htm

1. INSTALLATION

1.1. INSTALLATION UNDER UNIX

   Before installing the program libpcap (www.tcpdump.org) must be installed.
	Try running "sudo apt-get remove libpcap-dev".

   For installation run ./INSTALL

   Use ant, ant1, ant2, ... scripts instead of anettest.

1.2. INSTALLATION UNDER WINDOWS

   Before running the program WinPcap (www.winpcap.org) must be installed.
	Then you can just run program.
	
   Use file registry.reg. Before adding info from this file to registry you must set your own default parameters in it.
   The addition may be performed by clicking on file.

1.3. PROGRAM HOME FOLDER

   In home folder program will search for "headers" folder, "samples" folder, "traces" folder with headers, samples and trace files.

   For UNIX it's /usr/local/anettest.
   If you use another home folder it must be specified by -I option.

   For Windows it's folder from file "registry.reg".
   If you use another home folder it must be specified by -I option (or change "registry.reg" file).

   The current directory is also home directory (it has the highest priority when searching for referenced files).

2. FIRST START

	Full documentation is available on site http://anettest.sourceforge.net/
	where you can find a lot of examples how to use program.
	The same examples are also in samples folder of distribute.
	Better start watching them on site (with highlighting) then run, edit locally.
	   
   You may start from these

      generate_sequence_packets.fws
      waiting_packets.fws
      simple_sniffer.fws
      ask_mac.fws
      compare_mode.fws
      compare_mode1.fws
      tracing_packets.fws

   To watch the list of network interfaces run: anettest -i.
   IP address of interface may be used instead of original name.
   Use those names or IP addresses in option -d.

	All fields that you can reference while specifying a packet to send (wait)
	are in header files (header folder). 
	Usually you need to include those file but some fields are hard-coded in program.
	If you want to watch header files start with tcp_header.fws (contains more comments).

   Description of commands and some special values is available through help command:

      anettest -d eth0 help <name of command> exit 0
   or
      anettest -d eth0 help all exit 0    -  displays description for all commands

   For the list of defined fields use -k option

      anettest -d eth0 -k tcp

   For Context editor you may use "Options -> Environment options -> Execute keys".
   For some key: type the path to anettest program, then specify parameters: "all_headers help %w getch exit 0".
   Now you can press this key in editor and watch the description of command or field which cursor points to.

2.1. BASE HEADER

   There is a special header file among headers. Its name is "base.fws".
   It will be processed before any file explicitly specified by -f option.
   All useful global definitions may be contained in this file.

3. FOR DEVELOPERS

	3.1. SOME CONFIGURATION OPTION OF BUILD
		
		Two options: debug and logging.		
		e.g. configure --enable-debug --enable-log

	3.2. Compiling with MinGW.
	
		Add gcc/g++ and make packages to MinGw.
		Download binary WinPcap package and copy Lib\libwpcap.a to MinGW/lib.
		Download sources WinPcap package, run create_include.bat and copy created \WpdPack\Include\ to MinGW/include.
		Run ./configure && make.
		
	3.3. Extending by new custom code.

		See ExtendHelper in sources. You can add new commands, values' types, automatically calculated values (like tcpcrc), devices to access to network (or anything).
		
		
			