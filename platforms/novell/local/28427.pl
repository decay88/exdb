source: http://www.securityfocus.com/bid/19688/info

Novell Identity Manager is prone to an arbitrary command-execution vulnerability. 

A local attacker can exploit this issue to execute arbitrary commands with superuser privileges. Exploiting this issue allows attackers to completely compromise affected computers.

CMD="usermod -c $gecos"