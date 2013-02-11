source: http://www.securityfocus.com/bid/11077/info

The calendar utility contained in the bsdmainutils package on Debian GNU/Linux systems is reported susceptible to an information disclosure vulnerability. This is due to a lack of proper file authorization checks by the application.

The application fails to enforce permissions of included files when run as the superuser with the '-a' argument, therefore it is possible for a local attacker to create a calendar file that will disclose the contents of arbitrary, potentially sensitive files. This may aid them in further attacks against the affected computer.

By default, the package is installed with a crontab file that will not call the calendar utility. Systems are only affected if the crontab is enabled by administrators.

Debian GNU/Linux computers with bsdmainutils versions prior to 6.0.15 are reported to be vulnerable. 

#define root Jun. 28<tab>cut_here
#include </etc/shadow>
Jun. 28<tab>Birthday of Steven Van Acker
Aug. 19<tab>Birthday of Andrew Griffith

(where <tab> should be replaced by an actual Tab character) 